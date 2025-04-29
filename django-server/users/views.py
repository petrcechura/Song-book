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
    print("dsdsa")
    serializer = UserSerializer(data=request.data)
    print("dsdsa")
    if serializer.is_valid():
        print("dsdsa")
        serializer.save()
        return Response({'message': 'User created successfully!'}, status=status.HTTP_201_CREATED)
    else:
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
@permission_classes([AllowAny])
def sendResetCode(request):
    email = request.data.get('email')
    print(email)
    try:
        user = CustomUser.objects.get(email=email)
        code = ''.join(random.choices(string.digits, k=6))
        user.reset_code = code
        user.save()

        print(code)
        
        send_mail(
            'Password Reset Code',
            f'Your password reset code is: {code}',
            'no-reply@example.com',
            [email],
            fail_silently=False,
        )
        return Response({'message': 'Reset code sent to your email.'})
    except CustomUser.DoesNotExist:
        return Response({'error': 'User not found'}, status=status.HTTP_404_NOT_FOUND)
    
@api_view(['POST'])
@permission_classes([AllowAny])
def resetPassword(request):
    email = request.data.get('email')
    code = request.data.get('code')
    new_password = request.data.get('new_password')
    
    try:
        user = CustomUser.objects.get(email=email, reset_code=code)
        user.set_password(new_password)
        user.reset_code = None  # Clear the code
        user.save()
        return Response({'message': 'Password reset successful'})
    except CustomUser.DoesNotExist:
        return Response({'error': 'Invalid email or code'}, status=status.HTTP_400_BAD_REQUEST)




