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
-/+ - зум

Q, E - смена раскраски.
R - смена движка: AVX -> Naive -> Arrays -> AVX...
X - выход.
B - выход с запуском бенчмарка.

Также можно перемещать и увеличивать с помощью
мыши, зажимая левую кнопку мыши и двигая,
а также крутя колёсико.

## Тесты производительности
Использовалось 3 подхода:

### Наивная

Здесь мы просто расчитываем каждый пиксель по формуле
множества Мандельброта.

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

Здесь за раз рассчитывается по несколько пикселей за раз,
используя циклы и массивы.

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

Здесь используются AVX512 инструкции, позволяющие одновременно обрабатывать 8 пикселей одной инструкцией.

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

Прогнав по 100 раз каждую генерацию кадра, без её отрисовки,
были получены следующие результаты:

Реализация | Количество прогонов | Количество Тиков                    | Относительно предыдущего уроня оптимизации |
|:---------|:-------------------:|:------------------------------------|:-------------------------------------------|
Naive   -O0|100                |$66703973486 \approx 6.67 * 10^{10}$   |$1$ 
Arrays  -O0|100                |$125689457502 \approx 1.26 * 10^{11}$  |$1$
AVX512  -O0|100                |$19529772078 \approx 2.07 * 10^{10}$   |$1$

* $\frac{\text{Naive}}{\text{Arrays}} = 0.5307$
* $\frac{\text{Naive}}{\text{AVX}} = 3.416$
* $\frac{\text{Arrays}}{\text{AVX}} = 6.436$

Реализация | Количество прогонов | Количество Тиков                    | Относительно предыдущего уроня оптимизации |
|:---------|:-------------------:|:------------------------------------|:-------------------------------------------|
Naive   -O1|100                  |$32970065832 \approx 3.27 * 10^{10}$ |$0.494$
Arrays  -O1|100                  |$40606640412 \approx 4.15 * 10^{10}$ |$0.323$
AVX512  -O1|100                  |$5337173748 \approx 5.38 * 10^9$     |$0.273$

* $\frac{\text{Naive}}{\text{Arrays}} = 0.8119$
* $\frac{\text{Naive}}{\text{AVX}} = 6.177$
* $\frac{\text{Arrays}}{\text{AVX}} = 7.608$

Реализация | Количество прогонов | Количество Тиков                    | Относительно предыдущего уроня оптимизации |
|:---------|:-------------------:|:------------------------------------|:-------------------------------------------|
Naive   -O2|100                  |$30624959882 \approx 3.06 * 10^{10}$ |$0.929$
Arrays  -O2|100                  |$13951545387 \approx 1.40 * 10^{10}$ |$0.344$
AVX512  -O2|100                  |$4968688834 \approx 4.97 * 10^9$     |$0.931$

* $\frac{\text{Naive}}{\text{Arrays}} = 2.195$
* $\frac{\text{Naive}}{\text{AVX}} = 6.164$
* $\frac{\text{Arrays}}{\text{AVX}} = 2.808$

Реализация | Количество прогонов | Количество Тиков                    | Относительно предыдущего уроня оптимизации |
|:---------|:-------------------:|:------------------------------------|:-------------------------------------------|
Naive   -O3|100                  |$30484391570 \approx 3.05 * 10^{10}$ |$0.995$
Arrays  -O3|100                  |$12688068208 \approx 1.27 * 10^{10}$ |$0.909$
AVX512  -O3|100                  |$4845651135 \approx 4.85 * 10^9$     |$0.975$

* $\frac{\text{Naive}}{\text{Arrays}} = 2.403$
* $\frac{\text{Naive}}{\text{AVX}} = 6.291$
* $\frac{\text{Arrays}}{\text{AVX}} = 2.618$

![alt text](img/Graph.png)

## Вывод
До O2 наивная реализация оказывается быстрее массивной
из-за того, что массивы создают очень много накладных расходов
в виде счётчиков, jmp'ов, а также выделения памяти под
массивы на стеке. На O2 компилятор позволяет себе
векторизовать операции с массивами, но ему не удаётся
сделать это так же эффективно, как это можем мы, используя
интринсики.
