from django.shortcuts import get_object_or_404
from rest_framework.decorators import api_view, permission_classes
from rest_framework.response import Response
from rest_framework.permissions import AllowAny
from rest_framework import status
from .models import Song
from .serializers import SongSerializer
from googlesearch import search
from django.db.models import Q
import re
import locale

locale.setlocale(locale.LC_COLLATE, 'cs_CZ.UTF-8')  # Use Czech locale for sorting

## ROUTES ##
############

routeGet = {
    'Endpoint': 'songs/',
    'method': 'GET',
    'body': None,
    'description': 'Returns the array of songs'
}

@api_view(['GET'])
def getRoutes(request):
    routes = [
        routeGet,
    ]
    return Response(routes)

@api_view(['GET'])
def getSongs(request):
    songs = Song.objects.all()
    serializer = SongSerializer(songs, many=True)  # correct usage
    return Response(serializer.data, status=status.HTTP_200_OK)

@api_view(['POST'])
def createSong(request):
    data = request.data.copy()  # Make a mutable copy

    # Auto-assign ID as max existing ID + 1
    last_song = Song.objects.order_by('-id').first()
    new_id = (last_song.id + 1) if last_song else 1  # Start from 1 if table is empty

    data['id'] = new_id  # Overwrite/assign ID regardless of incoming data

    serializer = SongSerializer(data=data)

    if serializer.is_valid():
        serializer.save()  # id is now guaranteed to be our assigned new_id
        return Response(serializer.data, status=status.HTTP_201_CREATED)

    return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)

@api_view(['DELETE'])
def deleteSong(request, id):
    song = get_object_or_404(Song, id=id)  # Fixed lookup
    song.delete()
    return Response({'message': 'Song deleted successfully.'}, status=status.HTTP_204_NO_CONTENT)

@api_view(['DELETE'])
def deleteSongById(request):
    song_id = request.data.get('id')  # Expecting JSON body like {"id": 5}
    
    if song_id is None:
        return Response({'error': 'ID not provided in request body.'}, status=status.HTTP_400_BAD_REQUEST)

    song = get_object_or_404(Song, id=song_id)
    song.delete()
    return Response({'message': f'Song with id {song_id} deleted.'}, status=status.HTTP_204_NO_CONTENT)

@api_view(['PUT'])
def editSong(request, id):
    song = get_object_or_404(Song, id=id)

    serializer = SongSerializer(song, data=request.data, partial=True)  # partial=True allows partial updates

    if serializer.is_valid():
        serializer.save()  # Only provided fields will be updated
        return Response(serializer.data, status=status.HTTP_200_OK)

    return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)

@api_view(['GET'])
def findSongs(request):
    query = request.GET.get('query', None)
    field = request.GET.get('field', None)  # 'name' or 'author'

    if not query or field not in ['name', 'author']:
        return Response({'error': 'Please provide both query and field (name or author).'}, status=status.HTTP_400_BAD_REQUEST)

    try:
        regex = re.compile(query, re.IGNORECASE)
    except re.error:
        return Response({'error': 'Invalid regex pattern.'}, status=status.HTTP_400_BAD_REQUEST)

    songs = Song.objects.all()
    filtered = [song for song in songs if regex.search(getattr(song, field, ''))]

    serializer = SongSerializer(filtered, many=True)
    return Response(serializer.data, status=status.HTTP_200_OK)

@api_view(['POST'])
def sortSongs(request):
    sort_by = request.data.get('field', None)  # Should be 'name' or 'author'

    if sort_by not in ['name', 'author']:
        return Response({'error': 'Field must be "name" or "author".'}, status=status.HTTP_400_BAD_REQUEST)

    songs = list(Song.objects.all())
    songs.sort(key=lambda s: locale.strxfrm(getattr(s, sort_by, '')))  # Czech-friendly sort

    # Re-assign IDs in order: 0,1,2,...
    for new_id, song in enumerate(songs):
        song.id = new_id
        song.save()

    serializer = SongSerializer(songs, many=True)
    return Response(serializer.data, status=status.HTTP_200_OK)

ALLOWED_CHORD_SITES = [
    'pisnicky-akordy.cz',
    'supermusic.cz'
]

@api_view(['GET'])
@permission_classes([AllowAny])
def getChordsUrl(request):
    song_id = request.GET.get('song_id')

    if not song_id:
        return Response({'error': 'Song ID not provided.'}, status=status.HTTP_400_BAD_REQUEST)

    try:
        song = Song.objects.get(pk=song_id)

        if song.url:
            return Response({'url': song.url}, status=status.HTTP_200_OK)

        query = f"{song.author} {song.name} chords"
        results = search(query, num_results=5)  # Check top 5

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




