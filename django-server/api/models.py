from django.db import models

# Create your models here.


class Song(models.Model):
    id = models.IntegerField(primary_key=True)
    name = models.TextField()
    author = models.TextField()