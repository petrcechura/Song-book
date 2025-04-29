import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'package:http/http.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:flutter_app/song.dart';
import 'package:flutter_app/urls.dart';
import 'package:flutter_app/create.dart';
import 'package:flutter_app/update.dart';
import 'package:flutter_app/register.dart'; // <-- Added
import 'package:url_launcher/url_launcher.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  Future<bool> _checkLogin() async {
    final prefs = await SharedPreferences.getInstance();
    final token = prefs.getString('access_token');
    return token != null;
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Songbook',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepPurple),
        useMaterial3: true,
      ),
      home: FutureBuilder(
        future: _checkLogin(),
        builder: (context, snapshot) {
          if (snapshot.connectionState == ConnectionState.waiting) {
            return const Scaffold(
              body: Center(child: CircularProgressIndicator()),
            );
          }
          if (snapshot.data == true) {
            return const MyHomePage(title: 'Django Song-book Application');
          } else {
            return const LoginPage();
          }
        },
      ),
    );
  }
}

class LoginPage extends StatefulWidget {
  const LoginPage({super.key});

  @override
  State<LoginPage> createState() => _LoginPageState();
}

class _LoginPageState extends State<LoginPage> {
  final TextEditingController emailController = TextEditingController();
  final TextEditingController passwordController = TextEditingController();
  bool isLoading = false;
  String error = '';

  Future<void> login() async {
    setState(() {
      isLoading = true;
      error = '';
    });

    try {
      final response = await http.post(
        loginLsUrl,
        headers: {'Content-Type': 'application/json'},
        body: jsonEncode({
          'email': emailController.text,
          'password': passwordController.text,
        }),
      );

      if (response.statusCode == 200) {
        final data = jsonDecode(response.body);
        final prefs = await SharedPreferences.getInstance();
        await prefs.setString('access_token', data['access']);
        await prefs.setString('refresh_token', data['refresh']);

        if (!mounted) return;
        Navigator.pushReplacement(
          context,
          MaterialPageRoute(
            builder:
                (context) =>
                    const MyHomePage(title: 'Django Song-book Application'),
          ),
        );
      } else {
        setState(() {
          error = 'Invalid email or password';
        });
      }
    } catch (e) {
      setState(() {
        error = 'Connection failed';
      });
    } finally {
      setState(() {
        isLoading = false;
      });
    }
  }

  void goToRegister() {
    Navigator.push(
      context,
      MaterialPageRoute(builder: (context) => const RegisterPage()),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Login')),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            TextField(
              controller: emailController,
              decoration: const InputDecoration(labelText: 'Email'),
            ),
            const SizedBox(height: 12),
            TextField(
              controller: passwordController,
              decoration: const InputDecoration(labelText: 'Password'),
              obscureText: true,
            ),
            const SizedBox(height: 20),
            if (error.isNotEmpty)
              Text(error, style: const TextStyle(color: Colors.red)),
            ElevatedButton(
              onPressed: isLoading ? null : login,
              child:
                  isLoading
                      ? const CircularProgressIndicator()
                      : const Text('Login'),
            ),
            TextButton(
              onPressed: goToRegister,
              child: const Text('Create new account'),
            ),
          ],
        ),
      ),
    );
  }
}

enum SortBy { nameAsc, nameDesc, authorAsc, authorDesc }

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});
  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  Client client = http.Client();
  List<Song> songs = [];
  SortBy sortBy = SortBy.nameAsc; // <-- Add sorting state
  String? currentUserEmail;

  @override
  void initState() {
    super.initState();
    _loadUserInfo().then((_) => _fetchSongs());
  }

  Future<void> _loadUserInfo() async {
    final prefs = await SharedPreferences.getInstance();
    currentUserEmail = prefs.getString('user_email');
  }

  Future<Map<String, String>> _getAuthHeaders() async {
    final prefs = await SharedPreferences.getInstance();
    final token = prefs.getString('access_token');
    return {
      'Content-Type': 'application/json',
      'Authorization': 'Bearer $token',
    };
  }

  Future<void> _fetchSongs() async {
    songs = [];
    final headers = await _getAuthHeaders();

    try {
      final response = await client.get(fetchUrl, headers: headers);

      print(response.statusCode);

      if (response.statusCode == 200) {
        print('1');
        final decoded = utf8.decode(response.bodyBytes);
        print('2');
        List jsonResponse = json.decode(decoded);
        print('3');
        for (var element in jsonResponse) {
          print(element);
          songs.add(Song.fromMap(element));
          print('4');
        }
        print('5');
        _sortSongs(); // Apply sorting after fetching
        print('6');
        setState(() {});
        print('7');
      } else if (response.statusCode == 401) {
        _logoutAndRedirect(); // Token expired / invalid
      } else {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Error: ${response.statusCode}')),
        );
      }
    } catch (e) {
      print('e');
      print(e);
      ScaffoldMessenger.of(
        context,
      ).showSnackBar(const SnackBar(content: Text('Failed to load songs')));
    }
  }

  void _showManualUrlDialog(Song song) async {
    final TextEditingController urlController = TextEditingController();

    showDialog(
      context: context,
      builder: (BuildContext context) {
        return AlertDialog(
          title: const Text('Chords not found'),
          content: TextField(
            controller: urlController,
            decoration: const InputDecoration(
              hintText: 'Enter custom chords URL',
            ),
          ),
          actions: [
            TextButton(
              child: const Text('Cancel'),
              onPressed: () => Navigator.of(context).pop(),
            ),
            TextButton(
              child: const Text('Save'),
              onPressed: () async {
                final customUrl = urlController.text.trim();

                if (customUrl.isNotEmpty) {
                  final headers = await _getAuthHeaders();

                  final updateResponse = await http.put(
                    updateLsUrl(song.id),
                    headers: headers,
                    body: jsonEncode({'chords_url': customUrl}),
                  );

                  if (updateResponse.statusCode == 200) {
                    setState(() {
                      song.chordsUrl = customUrl;
                    });
                    ScaffoldMessenger.of(context).showSnackBar(
                      const SnackBar(content: Text('Custom URL saved!')),
                    );
                  } else {
                    ScaffoldMessenger.of(context).showSnackBar(
                      const SnackBar(
                        content: Text('Failed to save custom URL'),
                      ),
                    );
                  }
                }
                Navigator.of(context).pop();
              },
            ),
          ],
        );
      },
    );
  }

  void _openChordsUrl(String url) async {
    final uri = Uri.parse(url);
    if (await canLaunchUrl(uri)) {
      await launchUrl(uri);
    } else {
      ScaffoldMessenger.of(
        context,
      ).showSnackBar(const SnackBar(content: Text('Could not open URL')));
    }
  }

  void _findChordsForSong(Song song) async {
    final headers = await _getAuthHeaders();

    try {
      final response = await http.get(
        Uri.parse('$api/get_chords_url/?song_id=${song.id}'),
        headers: headers,
      );

      if (response.statusCode == 200) {
        final data = jsonDecode(response.body);
        final newUrl = data['url'];

        final updateResponse = await http.put(
          updateLsUrl(song.id),
          headers: headers,
          body: jsonEncode({'chords_url': newUrl}),
        );

        if (updateResponse.statusCode == 200) {
          setState(() {
            song.chordsUrl = newUrl;
          });
          ScaffoldMessenger.of(
            context,
          ).showSnackBar(const SnackBar(content: Text('Chords URL saved!')));
        } else {
          ScaffoldMessenger.of(context).showSnackBar(
            const SnackBar(content: Text('Failed to save chords URL')),
          );
        }
      } else if (response.statusCode == 404) {
        _showManualUrlDialog(song); // <-- When no valid URL is found
      } else {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Error: ${response.statusCode}')),
        );
      }
    } catch (e) {
      ScaffoldMessenger.of(
        context,
      ).showSnackBar(const SnackBar(content: Text('Error finding chords')));
    }
  }

  void _sortSongs() {
    switch (sortBy) {
      case SortBy.nameAsc:
        songs.sort(
          (a, b) => a.name.toLowerCase().compareTo(b.name.toLowerCase()),
        );
        break;
      case SortBy.nameDesc:
        songs.sort(
          (a, b) => b.name.toLowerCase().compareTo(a.name.toLowerCase()),
        );
        break;
      case SortBy.authorAsc:
        songs.sort(
          (a, b) => a.author.toLowerCase().compareTo(b.author.toLowerCase()),
        );
        break;
      case SortBy.authorDesc:
        songs.sort(
          (a, b) => b.author.toLowerCase().compareTo(a.author.toLowerCase()),
        );
        break;
    }
  }

  void _addSong() async {
    bool? created = await Navigator.push(
      context,
      MaterialPageRoute(builder: (context) => const CreatePage()),
    );
    if (created == true) {
      _fetchSongs();
    }
  }

  void _updateSong(Song song) async {
    bool? updated = await Navigator.push(
      context,
      MaterialPageRoute(builder: (context) => UpdatePage(song: song)),
    );
    if (updated == true) {
      _fetchSongs();
    }
  }

  void _deleteSong(int id) async {
    final headers = await _getAuthHeaders();

    final response = await client.delete(deleteLsUrl(id), headers: headers);

    if (response.statusCode == 204) {
      setState(() {
        songs.removeWhere((song) => song.id == id);
      });
    } else if (response.statusCode == 401) {
      _logoutAndRedirect(); // unauthorized
    } else {
      ScaffoldMessenger.of(
        context,
      ).showSnackBar(const SnackBar(content: Text('Failed to delete song')));
    }
  }

  Future<void> _logoutAndRedirect() async {
    final prefs = await SharedPreferences.getInstance();
    await prefs.remove('access_token');
    await prefs.remove('refresh_token');
    if (!mounted) return;
    Navigator.pushReplacement(
      context,
      MaterialPageRoute(builder: (context) => const LoginPage()),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Theme.of(context).colorScheme.inversePrimary,
        title: Text(widget.title),
        actions: [
          DropdownButton<SortBy>(
            value: sortBy,
            underline: const SizedBox(),
            icon: const Icon(Icons.sort, color: Colors.white),
            dropdownColor: Colors.deepPurple.shade100,
            onChanged: (SortBy? newValue) {
              if (newValue != null) {
                setState(() {
                  sortBy = newValue;
                  _sortSongs(); // Apply sorting immediately
                });
              }
            },
            items: const [
              DropdownMenuItem(value: SortBy.nameAsc, child: Text('Name ↑')),
              DropdownMenuItem(value: SortBy.nameDesc, child: Text('Name ↓')),
              DropdownMenuItem(
                value: SortBy.authorAsc,
                child: Text('Author ↑'),
              ),
              DropdownMenuItem(
                value: SortBy.authorDesc,
                child: Text('Author ↓'),
              ),
            ],
          ),
          IconButton(
            icon: const Icon(Icons.refresh),
            onPressed: _fetchSongs,
            tooltip: 'Refresh songs',
          ),
          IconButton(
            icon: const Icon(Icons.logout),
            onPressed: _logoutAndRedirect,
            tooltip: 'Logout',
          ),
        ],
      ),
      body: Column(
        children: [
          Container(
            padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 8),
            child: Row(
              children: const [
                SizedBox(
                  width: 60,
                  child: Text(
                    'ID',
                    style: TextStyle(
                      fontWeight: FontWeight.normal,
                      fontSize: 12,
                      color: Colors.grey,
                      letterSpacing: 1.2,
                    ),
                    textAlign: TextAlign.center,
                  ),
                ),
                Expanded(
                  flex: 4,
                  child: Text(
                    'NAME',
                    style: TextStyle(
                      fontWeight: FontWeight.normal,
                      fontSize: 12,
                      color: Colors.grey,
                      letterSpacing: 1.2,
                    ),
                    textAlign: TextAlign.center,
                  ),
                ),
                Expanded(
                  flex: 4,
                  child: Text(
                    'AUTHOR',
                    style: TextStyle(
                      fontWeight: FontWeight.normal,
                      fontSize: 12,
                      color: Colors.grey,
                      letterSpacing: 1.2,
                    ),
                    textAlign: TextAlign.center,
                  ),
                ),
                SizedBox(width: 50),
              ],
            ),
          ),
          const Divider(height: 1, color: Colors.grey),
          Expanded(
            child: ListView.builder(
              itemCount: songs.length,
              itemBuilder: (context, index) {
                final song = songs[index];
                return InkWell(
                  onTap: () => _updateSong(song),
                  child: Padding(
                    padding: const EdgeInsets.symmetric(
                      horizontal: 12,
                      vertical: 12,
                    ),
                    child: Row(
                      children: [
                        Container(
                          width: 60,
                          alignment: Alignment.center,
                          padding: const EdgeInsets.symmetric(vertical: 6),
                          decoration: BoxDecoration(
                            color: Colors.grey[300],
                            borderRadius: BorderRadius.circular(4),
                          ),
                          child: Text(
                            '${song.id}',
                            style: const TextStyle(fontWeight: FontWeight.bold),
                          ),
                        ),
                        const SizedBox(width: 16),
                        Expanded(
                          flex: 4,
                          child: Text(
                            song.name,
                            style: const TextStyle(fontWeight: FontWeight.bold),
                          ),
                        ),
                        Expanded(
                          flex: 4,
                          child: Text(
                            song.author,
                            style: const TextStyle(fontWeight: FontWeight.bold),
                          ),
                        ),
                        IconButton(
                          icon: Icon(
                            Icons.music_note,
                            color:
                                song.chordsUrl != null
                                    ? Colors.green
                                    : Colors.blue,
                          ),
                          onPressed:
                              song.chordsUrl != null
                                  ? () => _openChordsUrl(song.chordsUrl!)
                                  : () => _findChordsForSong(song),
                          tooltip:
                              song.chordsUrl != null
                                  ? 'View Chords'
                                  : 'Find Chords',
                        ),
                        IconButton(
                          icon: const Icon(Icons.delete, color: Colors.red),
                          onPressed: () => _deleteSong(song.id),
                        ),
                      ],
                    ),
                  ),
                );
              },
            ),
          ),
        ],
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: _addSong,
        tooltip: 'Add Song',
        child: const Icon(Icons.add),
      ),
    );
  }
}
