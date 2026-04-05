import os
import shutil
import glob
import tkinter as tk
from tkinter import messagebox, ttk

def select_from_list(parent, title, options, callback):
    """Открывает окно выбора из списка"""
    if not options:
        messagebox.showerror("Ошибка", "Нет доступных вариантов для выбора.")
        return

    top = tk.Toplevel(parent)
    top.title(title)
    top.geometry("600x250")
    top.transient(parent)
    top.grab_set()

    tk.Label(top, text="Выберите нужную папку:", font=("Arial", 10)).pack(pady=10)

    selected = tk.StringVar()
    combobox = ttk.Combobox(top, textvariable=selected, values=options, state="readonly", width=80)
    combobox.pack(pady=10)
    combobox.current(0)

    def on_select():
        value = selected.get()
        if value:
            top.destroy()
            callback(value)
        else:
            messagebox.showwarning("Внимание", "Выберите папку из списка.")

    tk.Button(top, text="Выбрать", command=on_select, font=("Arial", 10)).pack(pady=10)


def ask_overwrite_or_skip(parent, folder_path):
    """Спрашивает пользователя: перезаписать или пропустить? Возвращает 'overwrite', 'skip' или 'cancel'."""
    top = tk.Toplevel(parent)
    top.title("Папка уже существует")
    top.geometry("500x160")
    top.transient(parent)
    top.grab_set()

    tk.Label(
        top,
        text=f"Папка уже существует:\n{folder_path}\n\nЧто сделать?",
        font=("Arial", 10),
        justify=tk.CENTER,
        wraplength=480
    ).pack(pady=15)

    result = [None]

    def choose(action):
        result[0] = action
        top.destroy()

    btn_frame = tk.Frame(top)
    btn_frame.pack(pady=10)

    tk.Button(btn_frame, text="Перезаписать", command=lambda: choose('overwrite'), bg="#f44336", fg="white", width=12).grid(row=0, column=0, padx=10)
    tk.Button(btn_frame, text="Пропустить", command=lambda: choose('skip'), bg="#9E9E9E", fg="white", width=12).grid(row=0, column=1, padx=10)
    tk.Button(btn_frame, text="Отмена", command=lambda: choose('cancel'), bg="#607D8B", fg="white", width=12).grid(row=0, column=2, padx=10)

    top.wait_window()
    return result[0]


def copy_files():
    item_number = entry.get().strip()
    if not item_number:
        messagebox.showerror("Ошибка", "Введите номер изделия!")
        return

    try:
        order_number = item_number.split('.')[0] if '.' in item_number else item_number

        local_drawings_root = r"E:\Виктор\Чертежи"
        network_root = r"\\192.168.1.250\BestStudioFTP\free\ТО"

        # === Шаг 1: Найти папки заказа локально ===
        local_order_candidates = [
            f for f in os.listdir(local_drawings_root)
            if os.path.isdir(os.path.join(local_drawings_root, f)) and f.startswith(order_number)
        ]

        if not local_order_candidates:
            raise FileNotFoundError(f"Не найдена папка заказа '{order_number}' в {local_drawings_root}")

        def after_local_order_selected(local_order_name):
            order_folder_local = os.path.join(local_drawings_root, local_order_name)

            # === Шаг 2: Найти папку изделия ===
            item_candidates = [
                f for f in os.listdir(order_folder_local)
                if os.path.isdir(os.path.join(order_folder_local, f)) and f.startswith(item_number)
            ]

            if not item_candidates:
                messagebox.showerror("Ошибка", f"Не найдена папка изделия '{item_number}' в {order_folder_local}")
                return

            if len(item_candidates) == 1:
                item_folder_local = os.path.join(order_folder_local, item_candidates[0])
                proceed_to_network(item_folder_local)
            else:
                select_from_list(
                    root,
                    "Выберите папку изделия",
                    item_candidates,
                    lambda name: proceed_to_network(os.path.join(order_folder_local, name))
                )

        if len(local_order_candidates) == 1:
            after_local_order_selected(local_order_candidates[0])
        else:
            select_from_list(
                root,
                "Выберите папку заказа (локально)",
                local_order_candidates,
                after_local_order_selected
            )

    except Exception as e:
        messagebox.showerror("Ошибка", f"Произошла ошибка:\n{str(e)}")


def proceed_to_network(item_folder_local):
    """Продолжение после выбора локальной папки изделия"""
    item_number = entry.get().strip()
    order_number = item_number.split('.')[0] if '.' in item_number else item_number
    network_root = r"\\192.168.1.250\BestStudioFTP\free\ТО"

    try:
        network_order_candidates = [
            f for f in os.listdir(network_root)
            if os.path.isdir(os.path.join(network_root, f)) and f.startswith(f"№ {order_number}")
        ]

        if not network_order_candidates:
            raise FileNotFoundError(f"Не найдена сетевая папка '№ {order_number}' в {network_root}")

        def after_network_order_selected(network_order_name):
            network_order_path = os.path.join(network_root, network_order_name)
            drawings_folder_network = os.path.join(network_order_path, "чертежи")
            os.makedirs(drawings_folder_network, exist_ok=True)

            actual_item_folder_name = os.path.basename(item_folder_local)
            target_item_folder = os.path.join(drawings_folder_network, actual_item_folder_name)

            # 🔍 Проверка: если папка уже существует — спросить пользователя
            if os.path.exists(target_item_folder):
                choice = ask_overwrite_or_skip(root, target_item_folder)
                if choice == 'skip':
                    messagebox.showinfo("Пропущено", "Копирование отменено: папка уже существует.")
                    return
                elif choice == 'cancel':
                    return
                elif choice == 'overwrite':
                    # Удаляем старую папку
                    try:
                        shutil.rmtree(target_item_folder)
                    except Exception as e:
                        messagebox.showerror("Ошибка", f"Не удалось удалить существующую папку:\n{str(e)}")
                        return

            # Создаём новую папку
            os.makedirs(target_item_folder)

            # === Копирование файлов ===
            extensions_main = ['.pdf', '.xls', '.bln']
            extensions_laser = ['.dxf', '.igs']

            copied_files = []
            laser_files = []

            for ext in extensions_main + extensions_laser:
                pattern = os.path.join(item_folder_local, f"*{ext}")
                matched_files = glob.glob(pattern, recursive=False)
                for file in matched_files:
                    _, file_ext = os.path.splitext(file)
                    if file_ext.lower() in [e.lower() for e in extensions_laser]:
                        laser_files.append(file)
                    else:
                        copied_files.append(file)

            # Основные файлы
            for src in copied_files:
                dst = os.path.join(target_item_folder, os.path.basename(src))
                shutil.copy2(src, dst)

            # Лазерные файлы
            if laser_files:
                laser_folder = os.path.join(target_item_folder, "Лазер")
                os.makedirs(laser_folder)
                for src in laser_files:
                    dst = os.path.join(laser_folder, os.path.basename(src))
                    shutil.copy2(src, dst)

            # Отчёт
            report = f"✅ Успешно обработано изделие: {item_number}\n\n"
            report += f"Исходная папка: {item_folder_local}\n"
            report += f"Целевая папка: {target_item_folder}\n\n"
            report += f"Скопировано основных файлов: {len(copied_files)}\n"
            if laser_files:
                report += f"Файлов для лазера (.DXF/.IGS): {len(laser_files)} → в папку 'Лазер'\n"
            else:
                report += "Файлов для лазера не найдено.\n"

            messagebox.showinfo("Готово!", report)

        if len(network_order_candidates) == 1:
            after_network_order_selected(network_order_candidates[0])
        else:
            select_from_list(
                root,
                "Выберите сетевую папку заказа",
                network_order_candidates,
                after_network_order_selected
            )

    except Exception as e:
        messagebox.showerror("Ошибка", f"Ошибка при работе с сетевой папкой:\n{str(e)}")


# === Основное окно GUI ===
if __name__ == "__main__":
    root = tk.Tk()
    root.title("DrawFlow")
    root.geometry("480x170")
    root.resizable(False, False)

    tk.Label(root, text="Введите артикул изделия:", font=("Arial", 11)).pack(pady=12)
    entry = tk.Entry(root, width=32, font=("Arial", 12))
    entry.pack(pady=5)
    entry.focus()

    tk.Button(
        root,
        text="Выполнить",
        command=copy_files,
        font=("Arial", 11),
        bg="#4CAF50",
        fg="white",
        width=15,
        height=1
    ).pack(pady=15)

    root.mainloop()