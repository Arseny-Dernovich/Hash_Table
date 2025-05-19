import re

def split_words(input_file, output_file):
    with open(input_file, 'r', encoding='utf-8') as f:
        text = f.read()

    # Удалим всё кроме букв, апострофа и пробелов
    text = re.sub(r"[^\w\s']", ' ', text)
    text = text.lower()

    words = text.split()

    with open(output_file, 'w', encoding='utf-8') as f:
        for word in words:
            if len(word) < 31:
                f.write(word + '\n')

# Пример использования
split_words('input.txt', 'words.txt')
