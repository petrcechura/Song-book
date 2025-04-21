

class UserRouter:
    """
    A router to control all database operations on models in the 'users' application.
    Sends all user-related models to the 'user_db' database.
    """
    def db_for_read(self, model, **hints):
        if model._meta.app_label == 'users':
            return 'user_db'
        return None

    def db_for_write(self, model, **hints):
        if model._meta.app_label == 'users':
            return 'user_db'
        return None

    def allow_relation(self, obj1, obj2, **hints):
        # Allow relations if both objects are in the users app or both are not.
        if obj1._meta.app_label == 'users' and obj2._meta.app_label == 'users':
            return True
        elif obj1._meta.app_label != 'users' and obj2._meta.app_label != 'users':
            return True
        return False

    def allow_migrate(self, db, app_label, model_name=None, **hints):
        if app_label == 'users':
            return db == 'user_db'
        else:
            return db == 'default'

