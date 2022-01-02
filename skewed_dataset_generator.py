num_tx = 100
num_items = 100

# lower triangular matrix
def gen_variant_1():
    with open('datasets/varying_num_items/dataset_100_100_50_variant_1.txt', 'w') as f:
        tx_rows = []
        count = 0
        for i in range(0, num_tx):
            tx_rows.append((['1'] * (i + 1)) + (['0'] * (num_items - (i + 1))))
            count += i + 1
        for tx_row in tx_rows[:len(tx_rows) - 1]:
            f.write(' '.join(tx_row) + '\n')
        f.write(' '.join(tx_rows[-1]))
        print('Validating distribution of values: ' + str(count) + ' / ' + str(num_tx * num_items))

# step distribution
def gen_variant_2():
    with open('datasets/varying_num_items/dataset_100_100_50_variant_2.txt', 'w') as f:
        tx_rows = []
        count = 0
        for i in range(0, num_tx):
            if (i + 1) * 2 <= num_items:
                tx_rows.append((['1'] * ((i + 1) * 2)) + (['0'] * (num_items - ((i + 1) * 2))))
                count += ((i + 1) * 2)
            else:
                break
                '''
                tx_rows.append((['1'] * ((num_items - (i + 1)) * 2)) + (['0'] * (((i + 1) * 2) - num_items)))
                count += ((num_items - (i + 1)) * 2)
                '''
        tx_rows_reverse = tx_rows[:len(tx_rows) - 1][::-1]
        tx_rows += tx_rows_reverse
        count *= 2
        count -= num_items
        for tx_row in tx_rows:
            f.write(' '.join(tx_row) + '\n')
        f.write(' '.join(['0'] * num_items))
        print('Validating distribution of values: ' + str(count) + ' / ' + str(num_tx * num_items))

# gaussian/normal distribution
def gen_variant_3():
    with open('datasets/varying_num_items/dataset_100_100_50_variant_3.txt', 'w') as f:
        tx_rows = []
        count = 0
        for i in range(0, num_tx):
            if (i + 1) * 3 <= (num_items * 0.75):
                tx_rows.append((['1'] * ((i + 1) * 3)) + (['0'] * (num_items - ((i + 1) * 3))))
                count += ((i + 1) * 3)
            if (i + 1) > (num_items * 0.25) and (i + 1) <= (num_items * 0.5):
                tx_rows.append((['1'] * (int(num_items * 0.5) + i + 1)) + (['0'] * (num_items - (int(num_items * 0.5) + i + 1))))
                count += ((num_items * 0.5) + i + 1)
            if (i + 1) > (num_items * 0.5):
                break
        tx_rows_reverse = tx_rows[:len(tx_rows) - 1][::-1]
        tx_rows += tx_rows_reverse
        count *= 2
        count -= num_items
        for tx_row in tx_rows:
            f.write(' '.join(tx_row) + '\n')
        f.write(' '.join(['0'] * num_items))
        print('Validating distribution of values: ' + str(int(count)) + ' / ' + str(num_tx * num_items))

# exponential distribution
def gen_variant_4():
    with open('datasets/varying_num_items/dataset_100_100_50_variant_4.txt', 'w') as f:
        tx_rows = []
        count = 0
        for i in range(0, num_tx):
            if (i + 1) <= (num_items * 0.25):
                tx_rows.append((['1'] * (i + 1)) + (['0'] * (num_items - (i + 1))))
                count += i + 1
                offset = i + 1
            if (i + 1) > (num_items * 0.25) and (i + 1) <= (num_items * 0.5):
                offset += 3
                tx_rows.append((['1'] * offset) + (['0'] * (num_items - offset)))
                count += offset
            if (i + 1) > (num_items * 0.5):
                break
        tx_rows_reverse = tx_rows[:len(tx_rows) - 1][::-1]
        tx_rows += tx_rows_reverse
        count *= 2
        count -= num_items
        for tx_row in tx_rows:
            f.write(' '.join(tx_row) + '\n')
        f.write(' '.join(['0'] * num_items))
        print('Validating distribution of values: ' + str(int(count)) + ' / ' + str(num_tx * num_items))

def main():
    # gen_variant_1() 
    # gen_variant_2()
    # gen_variant_3()
    gen_variant_4()

if __name__ == '__main__':
    main()
