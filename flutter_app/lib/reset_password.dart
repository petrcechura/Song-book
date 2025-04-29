// reset_password.dart
import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'package:flutter_app/urls.dart';

class ResetPasswordPage extends StatefulWidget {
  final String email;
  ResetPasswordPage({super.key, required this.email});

  @override
  State<ResetPasswordPage> createState() => _ResetPasswordPageState();
}

class _ResetPasswordPageState extends State<ResetPasswordPage> {
  final TextEditingController codeController = TextEditingController();
  final TextEditingController passwordController = TextEditingController();
  bool isLoading = false;
  String error = '';
  String message = '';

  Future<void> resetPassword() async {
    setState(() {
      isLoading = true;
      error = '';
      message = '';
    });

    try {
      final response = await http.post(
        resetPasswordUrl, // This should be the URL you set up for resetting password
        headers: {'Content-Type': 'application/json'},
        body: jsonEncode({
          'email': widget.email,
          'code': codeController.text,
          'new_password': passwordController.text,
        }),
      );

      if (response.statusCode == 200) {
        setState(() {
          message = 'Password reset successful!';
        });
      } else {
        setState(() {
          error = 'Invalid reset code or email';
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

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Reset Password')),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            TextField(
              controller: codeController,
              decoration: const InputDecoration(labelText: 'Enter reset code'),
            ),
            const SizedBox(height: 12),
            TextField(
              controller: passwordController,
              obscureText: true,
              decoration: const InputDecoration(
                labelText: 'Enter new password',
              ),
            ),
            const SizedBox(height: 20),
            if (error.isNotEmpty)
              Text(error, style: const TextStyle(color: Colors.red)),
            if (message.isNotEmpty)
              Text(message, style: const TextStyle(color: Colors.green)),
            ElevatedButton(
              onPressed: isLoading ? null : resetPassword,
              child:
                  isLoading
                      ? const CircularProgressIndicator()
                      : const Text('Reset Password'),
            ),
          ],
        ),
      ),
    );
  }
}
