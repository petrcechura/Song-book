from django.db import models
from users.models import CustomUser  # assuming the user model is in 'users' app

class Song(models.Model):
    owner = models.ForeignKey(CustomUser, on_delete=models.CASCADE, related_name='songs')
    title = models.CharField(max_length=255)
    artist = models.CharField(max_length=255)
    url = models.CharField(max_length=500)
    id = models.IntegerField
    # add other fields you need (e.g., album, genre, etc.)

    def __str__(self):
        return f"{self.title} by {self.artist}"
