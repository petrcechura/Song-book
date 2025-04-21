// define retreive url as local loopback address 127.0.0.1:8000/lss
const String api = "http://127.0.0.1:8000";

Uri fetchUrl = Uri.parse("$api/songs");
Uri deleteLsUrl(int id) => Uri.parse("$api/songs/delete/$id");
Uri createLsUrl = Uri.parse("$api/songs/create");
Uri updateLsUrl(int id) => Uri.parse("$api/songs/edit/$id");
Uri loginLsUrl = Uri.parse("$api/login");
Uri registerLsUrl = Uri.parse("$api/register");
