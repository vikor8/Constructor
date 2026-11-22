import sqlite3
import os
import glob
import shutil

# --- НАСТРОЙКИ ---
SOURCE_DIR = "./old_dbs"          # Папка со старыми базами (названными по именам юзеров)
TARGET_DB  = "shared_workstats.db" # Имя новой общей базы
# -----------------

def create_new_schema(cursor):
    """Создает структуру новой общей базы данных."""
    # 1. Таблица пользователей
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password TEXT DEFAULT '123', -- Пароль по умолчанию для миграции
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    """)

    # 2. Таблица отправок в ТО (с привязкой к user_id)
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS sent_to_to (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            article TEXT NOT NULL,
            name TEXT,
            order_number TEXT,
            points REAL DEFAULT 0,
            sent_at DATETIME NOT NULL,
            FOREIGN KEY(user_id) REFERENCES users(id)
        )
    """)

    # 3. Таблица рабочих сессий (с привязкой к user_id)
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS work_sessions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            item_number TEXT NOT NULL,
            sketch_time_sec INTEGER DEFAULT 0,
            sketch_start DATETIME,
            sketch_end DATETIME,
            drawing_time_sec INTEGER DEFAULT 0,
            drawing_start DATETIME,
            drawing_end DATETIME,
            total_time_sec INTEGER DEFAULT 0,
            FOREIGN KEY(user_id) REFERENCES users(id)
        )
    """)

    # Индексы для ускорения фильтрации
    cursor.execute("CREATE INDEX IF NOT EXISTS idx_sent_user ON sent_to_to(user_id)")
    cursor.execute("CREATE INDEX IF NOT EXISTS idx_sent_date ON sent_to_to(sent_at)")
    cursor.execute("CREATE INDEX IF NOT EXISTS idx_ws_user ON work_sessions(user_id)")

def merge_databases():
    if not os.path.exists(SOURCE_DIR):
        print(f"Ошибка: Папка {SOURCE_DIR} не найдена.")
        return

    if os.path.exists(TARGET_DB):
        print(f"Внимание: Файл {TARGET_DB} уже существует. Переименовываю старый...")
        shutil.move(TARGET_DB, TARGET_DB + ".old")

    # Подключаемся к новой базе
    conn = sqlite3.connect(TARGET_DB)
    cursor = conn.cursor()
    create_new_schema(cursor)
    conn.commit()

    # Ищем все старые базы
    db_files = glob.glob(os.path.join(SOURCE_DIR, "*.db"))
    if not db_files:
        print("Не найдено ни одного .db файла для миграции.")
        return

    for db_file in db_files:
        username = os.path.basename(db_file).replace(".db", "").strip()
        print(f"\nОбработка пользователя: '{username}' (файл: {db_file})")

        # Добавляем пользователя в таблицу users (если его еще нет)
        cursor.execute("INSERT OR IGNORE INTO users (username) VALUES (?)", (username,))
        conn.commit()
        
        # Получаем ID пользователя
        cursor.execute("SELECT id FROM users WHERE username = ?", (username,))
        user_id = cursor.fetchone()[0]

        try:
            # Подключаем старую базу
            cursor.execute(f"ATTACH DATABASE '{db_file}' AS old_db")

            # Переносим sent_to_to
            cursor.execute(f"""
                INSERT INTO sent_to_to (user_id, article, name, order_number, points, sent_at)
                SELECT {user_id}, article, name, order_number, points, sent_at
                FROM old_db.sent_to_to
            """)
            sent_count = cursor.rowcount

            # Переносим work_sessions
            cursor.execute(f"""
                INSERT INTO work_sessions (user_id, item_number, sketch_time_sec, sketch_start, sketch_end, 
                                           drawing_time_sec, drawing_start, drawing_end, total_time_sec)
                SELECT {user_id}, item_number, sketch_time_sec, sketch_start, sketch_end, 
                       drawing_time_sec, drawing_start, drawing_end, total_time_sec
                FROM old_db.work_sessions
            """)
            ws_count = cursor.rowcount

            # Отключаем старую базу
            cursor.execute("DETACH DATABASE old_db")
            conn.commit()
            
            print(f"  -> Перенесено: {sent_count} записей 'в ТО', {ws_count} сессий работы.")

        except Exception as e:
            print(f"  -> Ошибка при переносе {db_file}: {e}")

    conn.close()
    print(f"\nГотово! Общая база создана: {TARGET_DB}")

if __name__ == "__main__":
    merge_databases()