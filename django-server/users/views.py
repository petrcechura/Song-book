from django.shortcuts import render
from .models import CustomUser
from rest_framework import serializers, status
from rest_framework.response import Response
from rest_framework.decorators import api_view
from rest_framework.permissions import AllowAny
from rest_framework_simplejwt.tokens import RefreshToken
from rest_framework import status
from rest_framework.decorators import api_view, permission_classes
from django.contrib.auth import authenticate
from django.core.mail import send_mail
import random
import string

class UserSerializer(serializers.ModelSerializer):
    class Meta:
        model = CustomUser
        fields = ['email', 'password']
        extra_kwargs = {'password': {'write_only': True}}

    def create(self, validated_data):
        user = CustomUser.objects.create_user(
            email=validated_data['email'],
            password=validated_data['password'],
        )
        return user

@api_view(['POST'])
@permission_classes([AllowAny])
def registerUser(request):
    print(request.data)
    serializer = UserSerializer(data=request.data)
    print(type(serializer))
    print('POST')
    print(serializer.is_valid())
    if serializer.is_valid():
        print('IS VALID!')
        serializer.save()
        return Response({'message': 'User created successfully!'}, status=status.HTTP_201_CREATED)
    else:
        print("NOT VALID!")
        return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)


@api_view(['POST'])
@permission_classes([AllowAny])
def loginUser(request):
    email = request.data.get('email')
    password = request.data.get('password')
    
    user = authenticate(email=email, password=password)
    if user:
        refresh = RefreshToken.for_user(user)
        return Response({
            'refresh': str(refresh),
            'access': str(refresh.access_token),
        })
    return Response({'error': 'Invalid credentials'}, status=status.HTTP_401_UNAUTHORIZED)

@api_view(['POST'])
def resetPassword(request):
    email = request.data.get('email')
    
    try:
        user = User.objects.get(email=email)
    except User.DoesNotExist:
        return Response({'error': 'User not found'}, status=status.HTTP_404_NOT_FOUND)
    
    reset_code = ''.join(random.choices(string.ascii_letters + string.digits, k=6))
    
    # Save the reset code to a model or send it directly to the user via email.
    # For this example, we're directly sending it via email.
    send_mail(
        'Password Reset Code',
        f'Your reset code is {reset_code}.',
        'noreply@example.com',
        [email],
    )
    
    return Response({'message': 'Reset code sent successfully!'}, status=status.HTTP_200_OK)


