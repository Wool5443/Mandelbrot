# Mandelbrot

## Компьютер
|                            |                            |
|----------------------------|----------------------------|
| Laptop                     | ASUS VivoBook 14 S433EA    |
| Distro                     | Arch Linxu x86_64          |
| Kernel                     | 6.8.4-zen1-1-zen           |
| Screen Resolution          | 1920x1080                  |
| CPU                        | Intel Core i5 1135G7       |
| RAM                        | 8gb 3200 MHz               |


## Фичи
Поддерживаются 4 раскраски.
В правом верхнем углу виден счётчик кадрова.

### Серая
![alt text](img/Grey.png)
### Серая более плавная
![alt text](img/GreyQuattor.png)
### Чёрно-белая
![alt text](img/BW.png)
### RGB Градиент
![alt text](img/Gradient.png)
### RG Градиент
![alt text](img/GradientRG.png)

## Управление
Стрелочки/WASD - перемещение
+/- - зум

Q, E - смена раскраски.
R - смена движка: AVX -> Naive -> Arrays -> AVX...
X - выход.
B - выход с запуском бенчмарка.

Также можно перемещать и увеличивать с помощью
мыши, зажимая левую кнопку мыши и двигая,
а также крутя колёсико.

## Тесты производительности
Использовалось 3 подхода:

### Наивный

Здесь мы просто расчитываем каждый пиксель по формуле
множества Мандельброта. Вычисления идут, пока точка не
выйдет за границы окружности радиуса R_MAX = 10.

$z_{i+1} = z_i ^ 2 + z_0$

```c++
const double xShift = (double)camera->w / 2.f;
const double yShift = (double)camera->h / 2.f;

for (int iy = 0; iy < camera->h; iy++)
{
    for (int ix = 0; ix < camera->w; ix++)
    {
        const double x0 = ((double)ix - xShift) / camera->scale - camera->x;
        const double y0 = ((double)iy - yShift) / camera->scale - camera->y;

        double x = x0, y = y0;

        int N = 0;

        for (; N < N_MAX; N++)
        {
            const double x2 = x * x;
            const double y2 = y * y;
            const double r2 = x2 + y2;

            if (r2 > R2_MAX)
                break;

            const double xy = x * y;

            x = x2 - y2 + x0;
            y = xy + xy + y0;
        }

        *(pixels + iy * camera->w + ix) = palette[N % NUMBER_OF_COLORS];
    }
}
```

### Arrays

В этой реализации, в каждой итерации цилка, мы просчитываем по 8 точек с
помощью циклов и массивов, чтобы компилятору было проще понять, как векторизовать наш код.

```c++
const double xShift = (double)camera->w / 2.f;
const double yShift = (double)camera->h / 2.f;

const double revScale = 1 / camera->scale;
double DX[SIMULTANEOUS_PIXELS] = {};
for (int i = 0; i < SIMULTANEOUS_PIXELS; i++) DX[i] = revScale * DX_FACTORS_ARRAY[i];

for (int iy = 0; iy < camera->h; iy++)
{
    uint32_t* pixelsRow = pixels;

    const double  y0 = ((double)iy - yShift) / camera->scale - camera->y;
    double Y0[SIMULTANEOUS_PIXELS] = {};
    for (int i = 0; i < SIMULTANEOUS_PIXELS; i++) Y0[i] = y0;

    for (int ix = 0; ix < camera->w; ix += SIMULTANEOUS_PIXELS)
    {
        const double  x0 = ((double)ix - xShift) / camera->scale - camera->x;

        double X0[SIMULTANEOUS_PIXELS] = {};
        for (int i = 0; i < SIMULTANEOUS_PIXELS; i++) X0[i] = x0;

        double X[SIMULTANEOUS_PIXELS], Y[SIMULTANEOUS_PIXELS];
        for (int i = 0; i < SIMULTANEOUS_PIXELS; i++) X[i] = X0[i];
        for (int i = 0; i < SIMULTANEOUS_PIXELS; i++) Y[i] = Y0[i];

        char     finished[SIMULTANEOUS_PIXELS] = {};
        uint32_t colors  [SIMULTANEOUS_PIXELS];
        for (int i = 0; i < SIMULTANEOUS_PIXELS; i++) colors[i] = palette[0];

        for (int n = 0; n < N_MAX; n++)
        {
            double X2[SIMULTANEOUS_PIXELS];
            for (int i = 0; i < SIMULTANEOUS_PIXELS; i++) X2[i] = X[i] * X[i];
            double Y2[SIMULTANEOUS_PIXELS];
            for (int i = 0; i < SIMULTANEOUS_PIXELS; i++) Y2[i] = Y[i] * Y[i];
            double XY[SIMULTANEOUS_PIXELS];
            for (int i = 0; i < SIMULTANEOUS_PIXELS; i++) XY[i] = X[i] * Y[i];

            char cmp[SIMULTANEOUS_PIXELS] = {};
            for (int i = 0; i < SIMULTANEOUS_PIXELS; i++)
                if (!finished[i] && X2[i] + Y2[i] < R2_MAX)
                    cmp[i] = 1;

            if (*(uint64_t*)finished) // since cmp is 8 bytes
                break;

            for (int i = 0; i < SIMULTANEOUS_PIXELS; i++)
                if (!finished[i] && !cmp[i])
                    colors[i] = palette[n];
            
            for (int i = 0; i < SIMULTANEOUS_PIXELS; i++)
                X[i] = X2[i] - Y2[i] + X0[i];
            for (int i = 0; i < SIMULTANEOUS_PIXELS; i++)
                Y[i] = XY[i] + Y0[i];
        }

        for (int i = 0; i < SIMULTANEOUS_PIXELS; i++)
            pixelsRow[i] = colors[i];
        pixelsRow += SIMULTANEOUS_PIXELS;
    }
    pixelsRow += camera->w;
}
```

### AVX512

Здесь используются AVX512 инструкции. Они могут за 1 такт выполнять операции сразу над 8 точками,
что в итоге может дать нам ускорение <= 8 раз.

```c++
const double xShift = (double)camera->w / 2.f;
const double yShift = (double)camera->h / 2.f;

const __m512d DX = _mm512_mul_pd (_mm512_set1_pd(1 / camera->scale), DX_FACTORS);

for (int iy = 0; iy < camera->h; iy++)
{
    uint32_t* pixelsRow = pixels;
    pixels += camera->w;

    const double  y0 = ((double)iy - yShift) / camera->scale - camera->y;
    const __m512d Y0 = _mm512_set1_pd(y0);

    for (int ix = 0; ix < camera->w; ix += SIMULTANEOUS_PIXELS)
    {
        const double  x0 = ((double)ix - xShift) / camera->scale - camera->x;

        const __m512d X0 = _mm512_add_pd(_mm512_set1_pd(x0), DX);

        __m512d X = X0, Y = Y0;

        __m512i  colors        = _mm512_set1_epi32(palette[0]);
        __mmask8 notYetInfinte = 0xFF;

        for (int n = 0; n < N_MAX; n++)
        {
            __m512d X2 = _mm512_mul_pd(X, X);
            __m512d Y2 = _mm512_mul_pd(Y, Y);
            __m512d XY = _mm512_mul_pd(X, Y);

            __mmask8 cmp                 = _mm512_cmplt_pd_mask(_mm512_add_pd(X2, Y2), R2_MAX_512);
            __mmask8 pixelsToChangeColor = cmp ^ notYetInfinte; // if pixel is finite not color
            notYetInfinte               &= cmp;

            colors = _mm512_mask_set1_epi32(colors, pixelsToChangeColor, palette[n]);

            if (!cmp) break;

            X = _mm512_add_pd(_mm512_sub_pd(X2, Y2), X0);
            Y = _mm512_add_pd(_mm512_add_pd(XY, XY), Y0);
        }

        _mm512_storeu_epi32(pixelsRow, colors);
        pixelsRow += SIMULTANEOUS_PIXELS;
    }
}
```

Прогнав 5 по 100 раз генерацию кадра без её отрисовки и усреднив,
были получены следующие результаты:

O Level		|Naive									        |Arrays									        |AVX
------------|-----------------------------------------------|-----------------------------------------------|---------------------------------------
0			|$6.3000 * 10^{10} \pm 0.1800 * 10^{10}$		|$1.1000 * 10^{11} \pm 0.0160 * 10^{11}$		|$1.8000 * 10^{10} \pm 0.1800 * 10^{10}$
1			|$3.0000 * 10^{10} \pm 0.0029 * 10^{10}$		|$3.7000 * 10^{10} \pm 0.0950 * 10^{10}$		|$5.1000 * 10^{9} \pm 0.0290 * 10^{9}$
2			|$2.8000 * 10^{10} \pm 0.0130 * 10^{10}$		|$1.3000 * 10^{10} \pm 0.0025 * 10^{10}$		|$4.7000 * 10^{9} \pm 0.1300 * 10^{9}$
3			|$2.8000 * 10^{10} \pm 0.0120 * 10^{10}$		|$1.2000 * 10^{10} \pm 0.0140 * 10^{10}$		|$4.5000 * 10^{9} \pm 0.1200 * 10^{9}$


![alt text](img/Graph.png)

## Вывод

Нам удалось ускорить отрисовку множества Мандельброта в $13.88$ раз, используя O3 и AVX инструкции.
