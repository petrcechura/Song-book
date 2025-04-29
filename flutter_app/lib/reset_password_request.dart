// reset_password_request.dart
import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'package:flutter_app/urls.dart';
import 'package:flutter_app/reset_password.dart';

class ResetPasswordRequestPage extends StatefulWidget {
  const ResetPasswordRequestPage({super.key});

  @override
  State<ResetPasswordRequestPage> createState() =>
      _ResetPasswordRequestPageState();
}

class _ResetPasswordRequestPageState extends State<ResetPasswordRequestPage> {
  final TextEditingController emailController = TextEditingController();
  bool isLoading = false;
  String error = '';
  String message = '';

  String savedEmail = '';

  Future<void> sendResetCode() async {
    setState(() {
      isLoading = true;
      error = '';
      message = '';
    });

    try {
      final response = await http.post(
        resetPasswordRequestUrl, // This should be the URL you set up for sending reset code
        headers: {'Content-Type': 'application/json'},
        body: jsonEncode({'email': emailController.text}),
      );

      if (response.statusCode == 200) {
        setState(() {
          message = 'Reset code sent. Please check your email!';
        });
      } else {
        setState(() {
          error = 'Failed to send reset code';
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

  void goToPasswordReset(String email) {
    Navigator.push(
      context,
      MaterialPageRoute(builder: (context) => ResetPasswordPage(email: email)),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Password Reset')),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            TextField(
              controller: emailController,
              decoration: const InputDecoration(labelText: 'Enter your email'),
            ),
            const SizedBox(height: 20),
            if (error.isNotEmpty)
              Text(error, style: const TextStyle(color: Colors.red)),
            if (message.isNotEmpty)
              Text(message, style: const TextStyle(color: Colors.green)),
            ElevatedButton(
              onPressed: isLoading ? null : sendResetCode,
              child:
                  isLoading
                      ? const CircularProgressIndicator()
                      : const Text('Send Reset Code'),
            ),
            ElevatedButton(
              onPressed:
                  isLoading
                      ? null
                      : () => goToPasswordReset(emailController.text),
              child:
                  isLoading
                      ? const SizedBox(
                        width: 18,
                        height: 18,
                        child: CircularProgressIndicator(strokeWidth: 2),
                      )
                      : const Text('Got Reset Code'),
            ),
          ],
        ),
      ),
    );
  }
}
