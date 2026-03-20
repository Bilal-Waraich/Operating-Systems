"""
Operating Systems
Assignment 3
Bilal Waraich
bwaraich@constructor.university
"""

import matplotlib.pyplot as plt

# Defining arrays to store the number of threads, execution time, and CPU usage obtained via: time ./pdi-numbers -s 1 -e 100000 -t [1 ... 12] -v  in the comman-line. 
threads = [1, 2, 4, 6, 8, 12]  # Number of threads used for the tests
execution_time = [0.020, 0.007, 0.008, 0.011, 0.010, 0.010]  # Execution times in seconds
cpu_usage = [92, 128, 235, 195, 212, 219]  # CPU usage percentages

# Main function to plot execution time and CPU usage
def main():
    # Creating a figure for the plots
    plt.figure(figsize=(10, 6))

    # Plotting execution time vs. number of threads
    plt.subplot(2, 1, 1)  # 2 rows, 1 column, 1st subplot
    plt.plot(threads, execution_time, marker='o', label='Execution Time (s)', color='blue')
    plt.xlabel('Number of Threads')  # Label for x-axis
    plt.ylabel('Execution Time (seconds)')  # Label for y-axis
    plt.title('Execution Time vs. Number of Threads')  # Title for the plot
    plt.xticks(threads)  # Setting x-ticks to the number of threads
    plt.grid(True)  # Adding grid lines
    plt.legend()  # Adding legend

    # Plotting CPU usage vs. number of threads
    plt.subplot(2, 1, 2)  # 2 rows, 1 column, 2nd subplot
    plt.plot(threads, cpu_usage, marker='o', label='CPU Usage (%)', color='orange')
    plt.xlabel('Number of Threads')  # Label for x-axis
    plt.ylabel('CPU Usage (%)')  # Label for y-axis
    plt.xticks(threads)  # Setting x-ticks to the number of threads
    plt.grid(True)  # Adding grid lines
    plt.legend()  # Adding legend

    # Adjusting the layout to fit into the figure area
    plt.tight_layout()  # Ensures that subplots fit into the figure area
    plt.show()  # Display the plots

# Entry point of the program
if __name__ == "__main__":
    main()  


"""
Answer for Part C:
The '-t' option impacts execution time by dividing the workload among multiple threads to allow
for parallel processing. Initially increasing the thread count reduces execution time: down to 0.007s
when there's 2 threads. However, as the number of threads increases the execution time shows diminishing returns.
This behaviour suggests that managing more threads limits the benefits of additional threads.
"""