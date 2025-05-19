import matplotlib.pyplot as plt

# Считываем данные
bucket_counts = []
with open('buckets_stat3.txt', 'r') as file:
    for line in file:
        bucket_counts.append(int(line.strip()))

# Строим график
plt.figure(figsize=(12, 6))
plt.plot(bucket_counts, marker='o', linestyle='none')  # Точки
plt.xlabel('Bucket index')
plt.ylabel('Number of elements in bucket')
plt.title('Distribution of words across hash table buckets')
plt.grid(True)
plt.show()
            