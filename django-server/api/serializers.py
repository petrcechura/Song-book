from rest_framework.serializers import ModelSerializer
from .models import Song
from users.models import CustomUser
from django.contrib.auth import get_user_model
from rest_framework import serializers

class SongSerializer(ModelSerializer):
    class Meta:
        model = Song
        fields = ['artist', 'title', 'id']
        optional_fields = ['url', 'id', 'owner']


class UserSerializer(serializers.ModelSerializer):
    class Meta:
        model = CustomUser
        fields = ['email', 'password']
        extra_kwargs = {'password': {'write_only': True}}

    def create(self, validated_data):
        return CustomUser.objects.create_user(**validated_data)



