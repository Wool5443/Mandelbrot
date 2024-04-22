from csv import reader

def value_to_str(val: float, eps_val: float) -> str:
    val_str   = f"{val:.2E}".replace("E+", "*10^{") + "}"

    val_num = float(val_str[:3])
    val_exp = float(val_str[-3:-1])

    return f"{val_num:.2f} * 10^{{{val_exp:.0f}}} \pm {eps_val:.2f}%"


def main():
    o_lev    = [0, 1, 2, 3]

    with open("tatata.csv", "r") as f:
        csv_read = list(reader(f, delimiter='\t', quotechar='"'))[1:]
    
    n = len(csv_read)
    
    naive      = [0] * n
    arrays     = [0] * n
    AVX        = [0] * n
    eps_naive  = [0] * n
    eps_arrays = [0] * n
    eps_AVX    = [0] * n

    for i, row in enumerate(csv_read):
        naive[i], eps_naive[i], arrays[i], eps_arrays[i], AVX[i], eps_AVX[i] = map(float, row[1:])

    ALIGN = "\t"

    # print(value_to_str(naive[0], d_naive[0]))

    if 1:
        with open("table.txt", "w") as f:

            print(f"O Level{ALIGN * 2}|Naive{ALIGN * 9}|Arrays{ALIGN * 9}|AVX", file=f)
            print("------------|---------------------------------------|---------------------------------------|-------------------------------", file=f)
            for i in range(n):
                print(f"{i}{ALIGN * 3}|${value_to_str(naive[i], eps_naive[i])}${ALIGN * 2}|", end="", file=f)
                print(f"${value_to_str(arrays[i], eps_arrays[i])}${ALIGN * 2}|", end="", file=f)
                print(f"${value_to_str(AVX[i], eps_AVX[i])}$", file=f)

main()
