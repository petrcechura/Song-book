import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'package:shared_preferences/shared_preferences.dart';
import 'package:flutter_app/song.dart';
import 'package:flutter_app/urls.dart';

class UpdatePage extends StatefulWidget {
  final Song song;
  const UpdatePage({super.key, required this.song});

  @override
  State<UpdatePage> createState() => _UpdatePageState();
}

class _UpdatePageState extends State<UpdatePage> {
  late TextEditingController nameController;
  late TextEditingController authorController;
  bool isLoading = false;
  String error = '';

  @override
  void initState() {
    super.initState();
    nameController = TextEditingController(text: widget.song.name);
    authorController = TextEditingController(text: widget.song.author);
  }

  Future<Map<String, String>> _getAuthHeaders() async {
    final prefs = await SharedPreferences.getInstance();
    final token = prefs.getString('access_token');
    return {
      'Content-Type': 'application/json',
      'Authorization': 'Bearer $token',
    };
  }

  void _updateSong() async {
    setState(() {
      isLoading = true;
      error = '';
    });

    final headers = await _getAuthHeaders();

    final response = await http.put(
      updateLsUrl(widget.song.id),
      headers: headers,
      body: jsonEncode({
        'title': nameController.text,
        'artist': authorController.text,
      }),
    );

    setState(() => isLoading = false);

    if (response.statusCode == 200) {
      if (!mounted) return;
      Navigator.pop(context, true);
    } else {
      setState(() {
        error = 'Failed to update song (${response.statusCode})';
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Edit Song')),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: [
            TextField(
              controller: nameController,
              decoration: const InputDecoration(labelText: 'Song Name'),
            ),
            const SizedBox(height: 10),
            TextField(
              controller: authorController,
              decoration: const InputDecoration(labelText: 'Author'),
            ),
            const SizedBox(height: 20),
            if (error.isNotEmpty)
              Text(error, style: const TextStyle(color: Colors.red)),
            ElevatedButton(
              onPressed: isLoading ? null : _updateSong,
              child:
                  isLoading
                      ? const CircularProgressIndicator()
                      : const Text('Update Song'),
            ),
          ],
        ),
      ),
    );
  }
}
