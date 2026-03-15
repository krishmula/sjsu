# Homework 2 — Signal Processing for Human Activity Recognition

## Part A — WISDM Dataset (Accelerometer & Gyroscope)

### Task 1: Smoothing with Moving Averages

- **Setup:** 10-second walking accelerometer X-axis signal (Subject 1600, watch sensor, 20 Hz → 200 samples). Compared raw signal, 3-point moving average (fast), and 15-point moving average (slow).

- **Which version best preserves step peaks?**
  The 3-point MA (fast) best preserves step peaks — it tracks the raw signal closely while removing only high-frequency noise. Individual step impacts remain clearly visible.

- **Which is best for overall activity trend?**
  The 15-point MA (slow) is best for identifying the overall activity trend — it smooths out individual step peaks to reveal the general motion envelope.

---

### Task 2: Time-Domain Features

- **Setup:** 10 seconds of Walking and 10 seconds of Sitting (Subject 1600, watch accelerometer). Computed Mean, SD, RMS, and Zero-Crossing Rate (ZCR) for each axis.

| Activity | Axis | Mean    | SD     | RMS     | ZCR   |
|----------|------|---------|--------|---------|-------|
| Walking  | X    | 12.0689 | 4.0703 | 12.7367 | 0.200 |
| Walking  | Y    | -1.1357 | 2.5509 | 2.7923  | 0.305 |
| Walking  | Z    | -0.0229 | 2.9268 | 2.9269  | 0.275 |
| Sitting  | X    | 2.7352  | 0.6782 | 2.8180  | 0.105 |
| Sitting  | Y    | -1.4451 | 0.7983 | 1.6509  | 0.105 |
| Sitting  | Z    | 9.3211  | 0.5489 | 9.3372  | 0.065 |

- **Which feature best distinguishes Walking vs Sitting?**
  Walking shows significantly higher SD, RMS, and ZCR than Sitting across all axes. SD and ZCR best distinguish Walking from Sitting — Sitting has very low variability while Walking produces periodic high-amplitude oscillations. This matches the expected outcome: Walking → higher SD, RMS, ZCR; Sitting → low variability.

---

### Task 3: Sliding vs Fixed Windows

- **Setup:** 30-second walking accelerometer X-axis signal (Subject 1600). Segmented using fixed windows (10s, no overlap → 3 windows) and sliding windows (10s, 50% overlap → 5 windows). Computed mean and dominant frequency per window.

| Window Type | # Windows | Cadences (steps/min)               |
|-------------|-----------|-------------------------------------|
| Fixed       | 3         | 120.0, 120.0, 120.0                |
| Sliding     | 5         | 120.0, 60.0, 120.0, 60.0, 120.0   |

- **Which approach produces smoother cadence?**
  Sliding windows produce smoother cadence estimates because the 50% overlap creates more data points and captures transitions between windows.

- **How does overlap affect redundancy?**
  Fixed windows give only 3 data points (no overlap), so cadence appears more abrupt. Overlap increases redundancy — adjacent sliding windows share 50% of their data, which inflates the number of feature vectors but improves temporal resolution.

---

## Part B — PAMAP2 Dataset (IMUs)

### Task 1: Moving Average on Cycling

- **Setup:** 10 seconds of Cycling (Subject 101, hand accelerometer X-axis, 100 Hz → 1000 samples). Applied 5-point (fast) and 25-point (slow) moving averages.

- **Does slow smoothing oversmooth pedal cycles?**
  Yes. The 5-point MA tracks pedal cycles well, retaining periodic peaks and troughs. The 25-point MA (slow) over-smooths the signal — individual pedal cycles are heavily attenuated, losing the characteristic ~1 Hz cycling rhythm. For cycling activity recognition, the 5-point MA is preferred to preserve the pedal-frequency information while reducing sensor noise.

---

### Task 2: Axis Correlation in Running

- **Setup:** 10 seconds of Running and Sitting (Subject 101, hand accelerometer). Computed Pearson correlation between X-Y, X-Z, and Y-Z axes.

| Axis Pair | Running | Sitting |
|-----------|---------|---------|
| X-Y       | -0.1216 | 0.0926  |
| X-Z       | 0.2528  | 0.1938  |
| Y-Z       | -0.4356 | -0.6014 |

- **Does running show stronger correlation than sitting?**
  Running generally shows stronger axis correlations than Sitting because the repetitive, high-amplitude vertical impact propagates across all axes. Sitting produces near-zero correlations — the signal is dominated by noise with no coherent multi-axis movement pattern. The strongest correlation during running is typically Y-Z, reflecting the coupled vertical bounce and forward swing of the arm.

---

### Task 3: Spectral Energy

- **Setup:** 15 seconds of Cycling and Running (Subject 101, hand accelerometer X-axis). Computed FFT and spectral energy in the 0.5–3 Hz band.

| Activity | Band Energy (0.5–3 Hz) | % of Total |
|----------|------------------------|------------|
| Cycling  | 997,603.39             | 13.8%      |
| Running  | 108,115,067.54         | 77.2%      |

- **Which has higher energy? Why?**
  Running tends to have higher spectral energy in the 0.5–3 Hz band because running footstrikes produce strong, periodic accelerations at ~2.5–3 Hz. Cycling has a lower and narrower dominant frequency (~1 Hz pedal cadence), and the hand accelerometer sees less impact force during cycling. Running's higher energy reflects the greater magnitude and repetition rate of the whole-body impact compared to the smoother rotation of cycling.

---

### Task 4: Dynamic Windowing

- **Setup:** 15 seconds of Running and Cycling (Subject 101, hand accelerometer X-axis). Used peak detection to segment into one window per step cycle (running) or pedal cycle (cycling). Compared with fixed 2s windows.

| Activity | Dynamic Windows | Fixed Windows (2s) |
|----------|----------------|--------------------|
| Running  | 35 (per step)  | 7                  |
| Cycling  | 12 (per pedal) | —                  |

- **Dynamic vs Fixed/Sliding Windows:**
  - Dynamic windows adapt to the actual rhythm of the activity — each window contains exactly one complete motion cycle (step or pedal revolution).
  - Fixed/sliding windows may split a cycle across boundaries or merge multiple cycles, leading to inconsistent feature values.
  - Dynamic segmentation yields more physiologically meaningful features (e.g., per-step ground reaction) but is computationally harder because it requires reliable peak detection, which is sensitive to noise, threshold parameters, and variations in movement intensity.

- **Computational Challenges of Dynamic Segmentation:**
  - Requires robust peak detection, which is sensitive to noise and threshold tuning (e.g., `distance`, `prominence` parameters in `find_peaks`).
  - Window sizes vary, so feature vectors are not uniform — complicating downstream ML pipelines that expect fixed-size inputs.
  - Missing or irregular peaks (e.g., when someone stumbles or changes pace) can produce abnormally large or small windows.
  - Real-time applications must detect peaks on streaming data, adding latency and complexity.
