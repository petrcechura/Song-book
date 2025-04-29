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
      name: map['title'],
      author: map['artist'],
      chordsUrl: map['url'],
    );
  }

  Map<String, dynamic> toMap() {
    return {'id': id, 'title': name, 'artist': author, 'url': chordsUrl};
  }
}
