# This is a sample Python script.
import sys
import numpy as np
import pandas as pd
#import mykmeanssp


def calc_min_dist(rows, np_data, mus, curr_num_of_mus):
    d_arr = np.zeros(rows)
    for l in range(rows):
        dl = min([np.sum((find_row_by_index(l, np_data)[1:] - mus[i][1:]) ** 2) for i in range(curr_num_of_mus)])
        d_arr[l] = dl

    return d_arr


def calc_probs(d_arr):
    d_sum = np.sum(d_arr)
    return d_arr / d_sum


def find_row_by_index(index, np_data):
    for row in np_data:
        if int(row[0]) == index:
            return row[1:]

    return None

def k_means_pp(k, max_iter, eps, input_1_filename, input_2_filename):
    np.random.seed(0)
    mus_indexes = []
    data_1 = pd.read_csv(input_1_filename, sep=",", header=None)
    data_2 = pd.read_csv(input_2_filename, sep=",", header=None)

    data = pd.merge(data_1, data_2, on=0)
    np_data = data.to_numpy()
    merged_input = open("merged_input","w")
    for row in np_data:
        lst_row = row[1:].tolist()
        mu = [(str('{:.4f}'.format(cord))) for cord in lst_row]
        merged_input.write(','.join(mu) +"\n")

    merged_input.close()
    rows = len(np_data)
    cols = len(np_data[0])

    indexes = np_data[:, 0].astype(int)
    indexes.sort()

    mus = np.zeros([k, cols-1])
    chosen_xi = np.random.choice(indexes)

    mus_indexes.append(chosen_xi)
    mus[0] = find_row_by_index(chosen_xi, np_data)


    for i in range(1, k):
        dist_arr = calc_min_dist(rows, np_data, mus, i)
        probs_arr = calc_probs(dist_arr)
        chosen_xi = np.random.choice(indexes, p=probs_arr)
        mus[i] = find_row_by_index(chosen_xi, np_data)
        mus_indexes.append(chosen_xi)

    mus_file = open('mus_file','w')
    for mu in mus:
        mu_str = [(str(cord)) for cord in mu.tolist()]
        mus_file.write(','.join(mu_str) +"\n")
    mus_file.close()

    mus_indexes_str = [(str(cord)) for cord in mus_indexes]
    print(','.join(mus_indexes_str))
    return "merged_input", 'mus_file'


def submit_args():
    if len(sys.argv) != 5 and len(sys.argv) != 6:
        print("Invalid Input!")
        return 0
    try:
        k = int(sys.argv[1])
        if len(sys.argv) == 6:

            max_iter = int(sys.argv[2])
            eps = float(sys.argv[3])
            input_1 = sys.argv[4]
            input_2 = sys.argv[5]
        else:
            max_iter = 300
            eps = sys.argv[2]
            input_1 = sys.argv[3]
            input_2 = sys.argv[4]

        # if not isinstance(k,int) or not isinstance(max_iter,int) or max_iter <= 0 or k <= 0:
        #     print("Invalid Input!")
        #     return 0

        if max_iter <= 0 or k <= 0 or eps <= 0:
            print("Invalid Input!")
            return 0

        f_input_1 = open(input_1)
        f_input_2 = open(input_2)
        f_input_1.close()
        f_input_2.close()

    except (ValueError, OSError):
        print("Invalid Input!")
        return 0

    return k, max_iter, eps, input_1, input_2


def main():
    file1 = "test_data\input_1_db_1.txt"
    file2 = "test_data\input_1_db_2.txt"
    # args = submit_args()
    args = [3, 333,0, file1, file2]
    if args == 0:
        return 0
    k, max_iter, eps, input_1, input_2 = args
    
    data_filename, mus_filename = k_means_pp(k, max_iter, eps, input_1, input_2)
    final_mus = mykmeanssp.fit(k,max_iter,eps,data_filename,mus_filename)
  #  print(final_mus)
    return 0


if __name__ == '__main__':
    main()

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
