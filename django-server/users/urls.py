from django.urls import path
from . import views

urlpatterns = [
    path('login', views.loginUser, name='login-user'),
    path('register', views.registerUser, name='register-user'),
    path('send-reset-code', views.sendResetCode),
    path('reset-password', views.resetPassword)
]

