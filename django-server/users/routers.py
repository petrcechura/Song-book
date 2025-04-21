

class UserDatabaseRouter:
    def db_for_read(self, model, **hints):
        # Define the logic to choose the database
        return 'user_db'  # Example: choose a user-specific DB

    def db_for_write(self, model, **hints):
        return 'user_db'
