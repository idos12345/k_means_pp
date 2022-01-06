import sys
import numpy as np
import pandas as pd
import mykmeanssp


def print_file(filename):
    f = open(filename)
    for line in f:
        print(line[:-1])
    f.close()


def calc_min_dist(rows, mus, curr_num_of_mus, row_by_index_d):
    d_arr = np.zeros(rows)
    for l in range(rows):
        dl = min([np.sum((row_by_index_d[l] - mus[i]) ** 2) for i in range(curr_num_of_mus)])
        d_arr[l] = dl
    return d_arr


def calc_probs(d_arr):
    d_sum = np.sum(d_arr)
    return d_arr / d_sum


def create_row_by_index_d(np_data):
    d = {}
    for row in np_data:
        d[int(row[0])] = row[1:]
    return d


def create_data_file_from_merged_inputs(filename, data):
    file = open(filename, "w")
    for row in data:
        lst_row = row[1:].tolist()
        mu = [(str(cord)) for cord in lst_row]
        file.write(','.join(mu) + "\n")
    file.close()


def create_mus_file(filename, data):
    file = open(filename, "w")
    for row in data:
        lst_row = row.tolist()
        mu = [(str(cord)) for cord in lst_row]
        file.write(','.join(mu) + "\n")
    file.close()


def find_mus(indexes, cols, np_data, k, rows):
    mus_indexes = []
    mus = np.zeros([k, cols - 1])
    chosen_xi = np.random.choice(indexes)
    mus_indexes.append(chosen_xi)
    row_by_index_d = create_row_by_index_d(np_data)

    mus[0] = row_by_index_d[chosen_xi]
    for i in range(1, k):
        dist_arr = calc_min_dist(rows, mus, i, row_by_index_d)
        probs_arr = calc_probs(dist_arr)
        chosen_xi = np.random.choice(indexes, p=probs_arr)
        mus[i] = row_by_index_d[chosen_xi]
        mus_indexes.append(chosen_xi)
    mus_indexes_str = [(str(cord)) for cord in mus_indexes]
    return mus_indexes_str, mus


def create_data_from_input_files(input_1_filename, input_2_filename):
    data_1 = pd.read_csv(input_1_filename, sep=",", header=None)
    data_2 = pd.read_csv(input_2_filename, sep=",", header=None)
    data = pd.merge(data_1, data_2, on=0)
    np_data = data.to_numpy()
    return np_data


def create_indexes(np_data):
    indexes = np_data[:, 0].astype(int)
    indexes.sort()
    return indexes


def k_means_pp(k, input_1_filename, input_2_filename):
    np.random.seed(0)

    np_data = create_data_from_input_files(input_1_filename, input_2_filename)
    create_data_file_from_merged_inputs("merged_input.txt", np_data)

    rows_number = len(np_data)
    cols_number = len(np_data[0])

    indexes_by_first_column = create_indexes(np_data)

    mus_indexes_str, mus = find_mus(indexes_by_first_column, cols_number, np_data, k, rows_number)
    create_mus_file("mus_file.txt", mus)

    print(','.join(mus_indexes_str))

    return "merged_input.txt", 'mus_file.txt'


def submit_args():
    if len(sys.argv) != 5 and len(sys.argv) != 6:
        print("Invalid Input!1")
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
            eps = float(sys.argv[2])
            input_1 = sys.argv[3]
            input_2 = sys.argv[4]

        if max_iter <= 0 or k <= 0 or eps < 0:
            print("Invalid Input!2")
            return 0
        
        f_input_1 = open(input_1)
        f_input_2 = open(input_2)
        f_input_1.close()
        f_input_2.close()

    except (ValueError, OSError):
        print("Invalid Input!3")
        return 0
    return k, max_iter, eps, input_1, input_2


def main():
    args = submit_args()
    if args == 0:
        return 0
    k, max_iter, eps, input_1, input_2 = args
    data_filename, mus_filename = k_means_pp(k, input_1, input_2)
    kmeans_succeed = mykmeanssp.k_means(k, max_iter, eps, data_filename, mus_filename)

    if kmeans_succeed == 0:
        print_file(mus_filename)
        return 0
    else:
        print("An Error Has Occurred") # mabey print twice this massege, one from C and one here
        return 1


if __name__ == '__main__':
    main()
