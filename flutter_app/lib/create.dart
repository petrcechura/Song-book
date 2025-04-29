import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'package:shared_preferences/shared_preferences.dart';
import 'package:flutter_app/urls.dart';

class CreatePage extends StatefulWidget {
  const CreatePage({super.key});

  @override
  State<CreatePage> createState() => _CreatePageState();
}

class _CreatePageState extends State<CreatePage> {
  final TextEditingController nameController = TextEditingController();
  final TextEditingController authorController = TextEditingController();
  bool isLoading = false;
  String error = '';
  String? currentUserEmail;

  Future<Map<String, String>> _getAuthHeaders() async {
    final prefs = await SharedPreferences.getInstance();
    final token = prefs.getString('access_token');
    return {
      'Content-Type': 'application/json',
      'Authorization': 'Bearer $token',
    };
  }

  void _createSong() async {
    setState(() {
      isLoading = true;
      error = '';
    });

    final prefs = await SharedPreferences.getInstance();
    currentUserEmail = prefs.getString('user_email');

    final headers = await _getAuthHeaders();

    final response = await http.post(
      createLsUrl,
      headers: headers,
      body: jsonEncode({
        'title': nameController.text,
        'artist': authorController.text,
      }),
    );

    setState(() => isLoading = false);

    if (response.statusCode == 201) {
      if (!mounted) return;
      Navigator.pop(context, true);
    } else {
      setState(() {
        error = 'Failed to create song (${response.statusCode})';
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Add New Song')),
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
              onPressed: isLoading ? null : _createSong,
              child:
                  isLoading
                      ? const CircularProgressIndicator()
                      : const Text('Save Song'),
            ),
          ],
        ),
      ),
    );
  }
}
