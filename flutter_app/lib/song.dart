// ignore_for_file: public_member_api_docs, sort_constructors_first

class Song {
  final int id;
  final String name;
  final String author;

  Song({required this.id, required this.name, required this.author});

  factory Song.fromMap(Map<String, dynamic> map) {
    return Song(id: map['id'], name: map['name'], author: map['author']);
  }

  Map<String, dynamic> toMap() {
    return {'id': id, 'name': name, 'author': author};
  }
}
