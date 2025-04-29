from django.shortcuts import get_object_or_404
from rest_framework.decorators import api_view, permission_classes
from rest_framework.response import Response
from rest_framework.permissions import IsAuthenticated
from rest_framework import status
from .models import Song
from .serializers import SongSerializer
from googlesearch import search
import re
import locale

locale.setlocale(locale.LC_COLLATE, 'cs_CZ.UTF-8')  # Czech locale

# ROUTES
@api_view(['GET'])
def getRoutes(request):
    routes = [
        {'Endpoint': 'songs/', 'method': 'GET', 'body': None, 'description': 'Returns the array of songs'}
    ]
    return Response(routes)

# Get all songs belonging to the logged-in user
@api_view(['GET'])
@permission_classes([IsAuthenticated])
def getSongs(request):
    songs = Song.objects.filter(owner=request.user)
    serializer = SongSerializer(songs, many=True)
    return Response(serializer.data, status=status.HTTP_200_OK)

# Create a new song for the logged-in user
@api_view(['POST'])
@permission_classes([IsAuthenticated])
def createSong(request):
    data = request.data.copy()
    serializer = SongSerializer(data=data)

    if serializer.is_valid():
        serializer.save(owner=request.user)  # Assign the song to the logged-in user
        return Response(serializer.data, status=status.HTTP_201_CREATED)
    
    return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)

# Delete a song belonging to the logged-in user
@api_view(['DELETE'])
@permission_classes([IsAuthenticated])
def deleteSong(request, id):
    song = get_object_or_404(Song, id=id, owner=request.user)
    song.delete()
    return Response({'message': 'Song deleted successfully.'}, status=status.HTTP_204_NO_CONTENT)

# Alternative delete by ID from request body
@api_view(['DELETE'])
@permission_classes([IsAuthenticated])
def deleteSongById(request):
    song_id = request.data.get('id')

    if song_id is None:
        return Response({'error': 'ID not provided.'}, status=status.HTTP_400_BAD_REQUEST)

    song = get_object_or_404(Song, id=song_id, owner=request.user)
    song.delete()
    return Response({'message': f'Song with id {song_id} deleted.'}, status=status.HTTP_204_NO_CONTENT)

# Edit a song belonging to the logged-in user
@api_view(['PUT'])
@permission_classes([IsAuthenticated])
def editSong(request, id):
    song = get_object_or_404(Song, id=id, owner=request.user)
    serializer = SongSerializer(song, data=request.data, partial=True)

    if serializer.is_valid():
        serializer.save()
        return Response(serializer.data, status=status.HTTP_200_OK)

    return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)

# Find songs by name or author belonging to the logged-in user
@api_view(['GET'])
@permission_classes([IsAuthenticated])
def findSongs(request):
    query = request.GET.get('query', None)
    field = request.GET.get('field', None)

    if not query or field not in ['name', 'author']:
        return Response({'error': 'Please provide both query and field (name or author).'}, status=status.HTTP_400_BAD_REQUEST)

    try:
        regex = re.compile(query, re.IGNORECASE)
    except re.error:
        return Response({'error': 'Invalid regex pattern.'}, status=status.HTTP_400_BAD_REQUEST)

    songs = Song.objects.filter(owner=request.user)
    filtered = [song for song in songs if regex.search(getattr(song, field, ''))]

    serializer = SongSerializer(filtered, many=True)
    return Response(serializer.data, status=status.HTTP_200_OK)

# Sort songs by name or author for the logged-in user
@api_view(['POST'])
@permission_classes([IsAuthenticated])
def sortSongs(request):
    sort_by = request.data.get('field', None)

    if sort_by not in ['name', 'author']:
        return Response({'error': 'Field must be "name" or "author".'}, status=status.HTTP_400_BAD_REQUEST)

    songs = list(Song.objects.filter(owner=request.user))
    songs.sort(key=lambda s: locale.strxfrm(getattr(s, sort_by, '')))

    serializer = SongSerializer(songs, many=True)
    return Response(serializer.data, status=status.HTTP_200_OK)

# Get chords URL for a song belonging to the logged-in user
ALLOWED_CHORD_SITES = [
    'pisnicky-akordy.cz',
    'supermusic.cz'
]

@api_view(['GET'])
@permission_classes([IsAuthenticated])
def getChordsUrl(request):
    song_id = request.GET.get('song_id')

    if not song_id:
        return Response({'error': 'Song ID not provided.'}, status=status.HTTP_400_BAD_REQUEST)

    try:
        song = Song.objects.get(pk=song_id, owner=request.user)

        if song.url:
            return Response({'url': song.url}, status=status.HTTP_200_OK)

        query = f"{song.author} {song.name} chords"
        results = search(query, num_results=5)

        for url in results:
            if any(allowed_site in url for allowed_site in ALLOWED_CHORD_SITES):
                song.url = url
                song.save()
                return Response({'url': url}, status=status.HTTP_200_OK)

        return Response({'error': 'No valid chords URL found in top 5 results.'}, status=status.HTTP_404_NOT_FOUND)

    except Song.DoesNotExist:
        return Response({'error': 'Song not found.'}, status=status.HTTP_404_NOT_FOUND)

    except Exception as e:
        return Response({'error': f'An error occurred: {str(e)}'}, status=status.HTTP_500_INTERNAL_SERVER_ERROR)
