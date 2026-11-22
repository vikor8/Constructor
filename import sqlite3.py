import sqlite3
import os
import glob
import shutil

SOURCE_DIR = "./old_dbs"           # Папка со старыми базами
TARGET_DB  = "shared_workstats.db" # Новая общая база

def merge_databases():
    if not os.path.exists(SOURCE_DIR):
        print(f"Ошибка: Папка {SOURCE_DIR} не найдена.")
        return

    if os.path.exists(TARGET_DB):
        print(f"Удаляем старую {TARGET_DB} для чистой миграции...")
        os.remove(TARGET_DB)

    conn = sqlite3.connect(TARGET_DB)
    cursor = conn.cursor()
    
    # Создаем таблицы
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL
        )
    """)
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS sent_to_to (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            article TEXT,
            name TEXT,
            order_number TEXT,
            points REAL,
            sent_at DATETIME,
            FOREIGN KEY(user_id) REFERENCES users(id)
        )
    """)
    conn.commit()

    db_files = glob.glob(os.path.join(SOURCE_DIR, "*.db"))
    if not db_files:
        print("Не найдено файлов .db в папке", SOURCE_DIR)
        return

    for db_file in db_files:
        username = os.path.basename(db_file).replace(".db", "").strip()
        print(f"\n--- Обработка файла: {os.path.basename(db_file)} (Пользователь: {username}) ---")

        # 1. Создаем пользователя
        cursor.execute("INSERT OR IGNORE INTO users (username) VALUES (?)", (username,))
        conn.commit()
        
        # 2. Получаем его ID
        cursor.execute("SELECT id FROM users WHERE username = ?", (username,))
        user_id = cursor.fetchone()[0]

        # 3. Безопасно подключаем старую базу
        try:
            cursor.execute(f"ATTACH DATABASE '{db_file}' AS old_db")
            
            # Проверяем, есть ли таблица в старой базе
            cursor.execute("SELECT name FROM old_db.sqlite_master WHERE type='table' AND name='sent_to_to'")
            if not cursor.fetchone():
                print(f"  [ПРОПУСК] В файле {db_file} нет таблицы sent_to_to.")
                continue

            # Переносим данные в ТО
            cursor.execute(f"""
                INSERT INTO sent_to_to (user_id, article, name, order_number, points, sent_at)
                SELECT {user_id}, article, name, order_number, points, sent_at
                FROM old_db.sent_to_to
            """)
            sent_count = cursor.rowcount
            print(f"  -> Перенесено записей 'В ТО': {sent_count}")

            # Если есть work_sessions, переносим и их
            cursor.execute("SELECT name FROM old_db.sqlite_master WHERE type='table' AND name='work_sessions'")
            if cursor.fetchone():
                cursor.execute(f"""
                    INSERT INTO work_sessions (user_id, item_number, sketch_time_sec, sketch_start, sketch_end, 
                                               drawing_time_sec, drawing_start, drawing_end, total_time_sec)
                    SELECT {user_id}, item_number, sketch_time_sec, sketch_start, sketch_end, 
                           drawing_time_sec, drawing_start, drawing_end, total_time_sec
                    FROM old_db.work_sessions
                """)
                print(f"  -> Перенесено рабочих сессий: {cursor.rowcount}")

            conn.commit()

        except Exception as e:
            print(f"  [ОШИБКА] При обработке {db_file}: {e}")
        finally:
            # ВАЖНО: Всегда отключаем базу, даже если была ошибка!
            try:
                cursor.execute("DETACH DATABASE old_db")
            except:
                pass

    conn.close()
    print(f"\nГотово! Все данные сохранены в {TARGET_DB}")

if __name__ == "__main__":
    merge_databases()