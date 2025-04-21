from django.urls import path
from . import views

urlpatterns = [
    path('', views.getRoutes),
    path('songs/', views.getSongs, name="get-songs"),
    path('songs/create', views.createSong, name="create-song"),
    path('songs/delete/<int:id>', views.deleteSong, name="delete-song"),
    path('songs/delete', views.deleteSongById, name="delete-id-song"),
    path('songs/edit/<int:id>', views.editSong, name="edit-song"),
    path('songs/find', views.findSongs, name="find-songs"),
    path('songs/sort', views.sortSongs, name="sort-songs"),
    path('get_chords_url/', views.getChordsUrl, name="get-chords-url")
]

