# This is a sample Python script.
import sys
import numpy as np
import pandas as pd
import mykmeanssp


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
            return row

    return None


def k_means_pp(k, max_iter, eps, input_1_filename, input_2_filename):
    np.random.seed(0)

    data_1 = pd.read_csv(input_1_filename, sep=",", header=None)
    data_2 = pd.read_csv(input_2_filename, sep=",", header=None)

    data = pd.merge(data_1, data_2, on=0)
    np_data = data.to_numpy()

    merged_input = open("merged_input","w")
    for row in np_data:
        merged_input.write(row[1:]+"\n")

    rows = len(np_data)
    cols = len(np_data[0])

    mus = np.zeros([k, cols])
    chosen_row = np.random.choice(range(rows))
    mus[0] = find_row_by_index(chosen_row, np_data)


    for i in range(1, k):
        dist_arr = calc_min_dist(rows, np_data, mus, i)
        probs_arr = calc_probs(dist_arr)
        chosen_row = np.random.choice(range(rows), p=probs_arr)
        mus[i] = find_row_by_index(chosen_row, np_data)

    print(mus)

    return merged_input, mus


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
    file1 = "test_data\input_2_db_1.txt"
    file2 = "trst_data\input_2_db_2.txt"
    # args = submit_args()
    args = [7, 0, file1, file2]
    if args == 0:
        return 0
    k, max_iter, eps, input_1, input_2 = args
    
    merged_input, line_index = k_means_pp(k, max_iter, eps, input_1, input_2)
    final_mus =  mykmeanssp.fit(k,max_iter,eps,merged_input,line_index)
    print(final_mus)
    return 0


if __name__ == '__main__':
    main()

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
