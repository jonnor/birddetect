
Technical report

http://dcase.community/challenge2018/submission

## Notes

Technical reports are not peer-reviewed.

not necessary to follow closely the scientific publication structure (for example there is no need for extensive literature review)
should contain sufficient description of the system

Please report the system performance using the provided cross-validation setup or development set, according to the task. 
Describe the task specific parameters in the report.

## Audiences

Paper reviewers. N/A, not peer-reviewed.
Journal editors. N/A, not to be published in journal.
Challenge host. Want a wide range of interesting,diverse results
Other challenge submitters. Want

Machine Learning practicioners. 
Embedded software developers.
Bird ecologists. 

Assume background knowledge in: machine learning, acoustic event detection
not so much knowledge in: microcontrollers, wireless sensor networks

## Title
DCASE2018 Bird Audio Detection on a microcontroller

using summarized melspecrograms with Random Forests
for continous acoustic monitoring
in Wireless Sensor Networks

## Goal
Evaluate whether chosen method can be suitable for
Bird Audio Detection in microcontroller-based Wireless Sensor network
Result so far: Needs more work on generalization

## Contributions

- approach can run with compute resources provided by modern microcontroller
- approach can support sleep for energy-bound devices (battery, energy harvesting)
- key considerations to be suitable for continious acoustic monitoring in a Wireless Sensor Network
- Basic melmax RF does not generalize well

## Outline

- Abstract

- Introduction.
Why was this done, relevant background

Perform inference on a low-cost microcontroller-based sensor.
running on battery or energy harvesting (solar power)

Such a system can choose to only transmit samples with a high probability of containing birds for detailed classification.
It may also be possible for system to provide an automated bird cencus estimation.
pre-annotate recorded samples , 

Machine learning methods that are resource efficient, in compute and memory.
Ideally the system can do the computations much faster than in real-time, and will spend the remaining time in sleep mode to conserve energy.

To avoid needing a time-consuming training period for every new installation,
the system must also generalize OK to new conditions.

The 2018 edition\cite{} of the DCASE challenge in Bird Audio Detection\cite{} focuses on generalization.


- System description.
mels specrogram, summarization, normalization, RF

linear instead of log, noise robustness. CITATION
max instead of mean/stdev, easier to implement.
parameters. frame size, n_mels
number of trees, split criterion

Python,sklearn CITE,librosa CITE
C99,floating-point,single-precision
emtrees CITE

Espressif ESP32 CITE. 32-bit microcontroller with integrated WiFi and Bluetooth Low Energy. 
Could alternatively have used a ARM Cortex M4F

Available as a module (ex ESP32-WROOM-32) for under 5 USD (in units of 1).
https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf
Arduino IDE 1.8.5 with arduino-esp32


- Evaluation.
cross-validation, per-testset
AUC ROC scores
OK of test sets.
Bad on evalation set.
Failure to generalize
Possible reasons
Inadequate normalization
Poor handling of different SNR

Memory usage. RAM
Program memory. Increase over baseline
Evaluation time. % of realtime.
Measured by sending frames of samples over MQTT. Transmission time is ignored.
- Conclusions.

Improve generalization
Implement realtime


Tables

Figures
- system pipeline. Collect samples, frame, summarize, predict



