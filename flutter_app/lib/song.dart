class Song {
  final int id;
  final String name;
  final String author;
  String? chordsUrl; // New field

  Song({
    required this.id,
    required this.name,
    required this.author,
    this.chordsUrl,
  });

  factory Song.fromMap(Map<String, dynamic> map) {
    return Song(
      id: map['id'],
      name: map['name'],
      author: map['author'],
      chordsUrl: map['url'], // Django should return this!
    );
  }

  Map<String, dynamic> toMap() {
    return {'id': id, 'name': name, 'author': author, 'chords_url': chordsUrl};
  }
}
