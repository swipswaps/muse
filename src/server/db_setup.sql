CREATE TABLE artist(
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	name VARCHAR(256) NOT NULL
);

CREATE TABLE album(
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	name VARCHAR(256) NOT NULL,
	year INTEGER
);

CREATE TABLE song(
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	name VARCHAR(256) NOT NULL,
	album_id INTEGER,
	artist_id INTEGER,
	track_num INTEGER,
	genre VARCHER(256),
	FOREIGN KEY (album_id) REFERENCES album(album_id),
	FOREIGN KEY (artist_id) REFERENCES artist(artist_id)
);

