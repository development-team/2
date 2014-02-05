##Artificial emotions via virtual neuromodulators

## Introduction

Biggest problem of artificial intelligence is that we don't know what is natural intelligence. The only example of intelligence that we observe today is human intelligence. Unfortunately there are not aliens that would be able to demonstrate the other way (non-human) intelligence could evolve through the years. Rosalind Picard in her article indicated: "There  may  exist  a  kind  of  alien  intelligent  living  system,  something we’ve  never  encountered,  which  achieves  its  intelligence  without  having  anything  like emotion.      Although  humans  are  the  most  marvellous  example  of  intelligence  we  have, and  we  wish  to  build  systems  that  are  natural  for  humans  to  understand,  these  reasons
for building human-like systems should not limit us to thinking only of human abilities." [affectivecomputingchallanges]

There are several domains that still remains unclear: creativity, intuition, insight and consciousness that prevents us from answering the question: "How David Lynch could create Mulholland Drive or Picasso could create Guernica?".
We have started from possibly first emerged during evolution and most low level component of intelligence: the emotions.

Turing stated in his [intelligent_machinery] that idea of intelligent machines "cannot be wholly ignored, because the idea of 'intelligence' is itself emotional rather than mathematical".
Marvin Minsky in his book "The emotion machine" proposed that emotions are inseparable from thinking "Emotional thinking: A flash of impatience or anger can cut through what seems like a hopelessly tangled knot. Each such ‘emotional way to think' is a different way to deal with things, and some can increase your persistence or courage, while others can help you simplify things. In any case, after each such change, you may still want to pursue some similar goals, but now you'll see them from new points of view — because each switch to a new Way to Think may initiate a large-scale cascade. Then, depending on how long those changes persist, you (or your friends) might recognize this as a change in your emotional state".

This article is dedicated to emotional aspects of human thinking and influence of emotions on computational processes in Marvin Minsky cognitive architecture [emotionmachine]. It could be considered as base of computational emotional thinking framework. This kind of framework could be used in several domains like:

1. Advertisement
1. Emotional behavior simulations
1. Robotics
1. Intellectual assistants
1. Estimating human behaviour
1. Nursing software and robotics.

## Bases

Starting from the top we first reviewed several psychological models of emotions. Then we tried to understand the low level nature of emotions that brought us to neuro-scientific base of emotions. As we got the picture of human emotional processes we mapped them to cognitive architecture to gain AI basis. This model was depicted in the picture below: 

![3 bases of theory](3_bases.png)

First base is evolutionary psychology theory of Plutchik [natureofemotions]. We used main emotional process (feedback loops) from it and adopted it to Model of six [emotionmachine] 6 thinking levels of Marvin Minsky cognitive architecture. We used "Wheel of emotions" [natureofemotions] as model for subjective conscious emotions and high level emotions.
Second base is theory of affects by Tomkins [primer_affect_psychology, tomkins1, tomkins2, tomkins3], we used it as the base for low level non-conscious emotions and low level appraisal.
Neuro-physiological base is Lovheim theory of neuromodulatory base of emotions [cubeofemotions]. We used "Cube of emotions" as main low level ("hardware") mechanism of emotional processes.
All theories described above were mapped to Marvin Minsky's cognitive architecture described in his book "The emotion machine" [emotionmachine].

##Emotional feedback loops

Plutchik created the three dimensional model of emotions [natureofemotions] called "Wheel of emotions" He used 8 basic emotions grouped in pairs:

1. Joy - sorrow
1. Anger - fear
1. Acceptance - disgust
1. Surprise - expectancy

We interpret these basic emotions as low level emotional conscious reactions that are close to innate non-conscious affects described by Tomkins[primer_affect_psychology]:

1. Enjoyment/Joy
1. Interest/Excitement
1. Surprise
1. Anger/Rage
1. Disgust
1. Distress/Anguish
1. Fear/Terror
1. Shame/Humiliation

We suppose that innate non-conscious affects and their appraisals could use different much more quick and simple mechanisms than developed, trained during life time conscious emotions, appraisals and coping. This way we use two models: affect theory for innate quick emotional reactions and "wheel of emotions" for conscious low level and high level emotions. According to Plutchik theory basic emotions could be mixed like colors into high level emotions listed below:

1. Love
1. Submission
1. Awe
1. Disapproval
1. Remorse
1. Contempt
1. Aggressiveness
1. Optimism

For example joy + acceptance = love; acceptance + fear = submission; anger + expectancy = aggressiveness.

This could be understood as base for subjective picture of emotions. 

![orchestra of emotions](orchestra_of_emotions_agi.png)

This is an example of emotional feedback loops [natureofemotions] represented in 4 layers of Marvin Minsky "model of six", that contains following layers:

1. Instinctive reactions
1. Learned reactions
1. Deliberative
1. Reflective thinking
1. Self-reflective thinking
1. Self-conscious reflections (is not shown on diagram).

We correspond instinctive reactions layer with non-conscious, innate, affective reactions that mainly takes palace in: spinal cord, hypothalamus and amygdala. This way any stimulus is been processed firstly non-consciously, this is shown as affective appraisal rectangle. Affective appraisal triggers neuromodulation. Actually neuromodulation is not the result of appraisal, but non-conscious appraisal is accompanied by neuromodulation on the way from spinal cord to hypothalamus and amygdala. 

##Neuromodulatory basis of artificial emotions

1. Nor-adrenaline
1. Dopamine
1. Serotonin

...

1. Enjoyment/Joy
1. Interest/Excitement
1. Surprise
1. Anger/Rage
1. Disgust
1. Distress/Anguish
1. Fear/Terror
1. Shame/Humiliation

...

According to [emotionsbraintorobot] there are four following neuronal systems involved in to emotional processing:

1. Spinal cord
1. Hypothalamus
1. Amygdala
1. Frontal cortex, cingulate cortex

We roughly correspond non-conscious instinctive reactions layer of "model of six" [emotionmachine] with spinal cord, hypothalamus and amygdala, while conscious processes and learned reactions, deliberative thinking, reflective thinking, self-reflective thinking, self-conscious reflections with frontal and cingulate cortex. 

##Neuromodulators to computing parameters mapping 

![parameter mapping](cube_of_parameters_agi.png)

1. Generic:
   2. CPU power: noradrenaline
   2. Memory distribution (attention): noradrenaline
   2. Learning: serotonin, dopamine
   2. Storage: serotonin, dopamine
1. Decision making/reward processing:
   2. Confidence: serotonin
   2. Satisfaction: serotonin
   2. Motivation, wanting: dopamine
   2. Risky choices inclination: noradrenaline
   2. Number of options to process: noradrenaline

Parameters are grouped in two folders: most obvious computing system parameters (generic):
*CPU power*(computing processes distribution or load balancing) is influenced by noradrenaline the higher is noradrenaline more computing processes should be concentrated on current activity.
*Working memory(short term)* distribution is influenced by noradrenaline as neurotransmitter regulating attention.
*Learning* is impacted by serotonin and dopamine: dopamine plays major role in activation of previously remembered patterns and serotonin in pattern generation.
*Storage* management (long term memory) is influenced both by serotonin and dopamine, higher concentrations of both neurotransmitters the better action is remembered(less probability to forget).

Second group contains parameters that influence decision making in probabilistic reasoning system. This reasoning is done mainly in deliberation and learned reaction layers.
Parameters: confidence, satisfaction, risky are used to highlight actions stored(remembered).
*Confidence and satisfaction* of the system is directly influenced by serotonin higher serotonin more confident is the system.
System is more *motivated* under influence of dopamine.
System tends to choose *risky* actions under impact of noradrenaline.
Noradrenaline makes system use less *number of options* in width and depth to be processed during reasoning.


##Appraisal and coping 

Picture on base of emotinal feedback(orchestra of emotions).

##High level and low level emotions

1. **G(Optimism Intensity)** = 1.0 serotonin  + 1.0 dopamine + 0.5 noradrenaline;
1. **G(Love Intensity)** = 0.75 serotonin + 1.0 dopamine + 0.0 noradrenaline;
1. **G(Submission Intensity)** = 0.25 serotonin + 1.0 dopamine + 0.0 noradrenaline;
1. **G(Awe Intensity)** = 0.5 serotonin + 0.5 dopamine + 0.5 noradrenaline;
1. **G(Disapproval Intensity)** = 0.5 serotonin + 0.0 dopamine + 1.0 noradrenaline;
1. **G(Remorse Intensity)** = 0.0 serotonin + 0.0 dopamine + 0.0 noradrenaline;
1. **G(Contempt Intensity)** = 0.5 serotonin + 0.5 dopamine + 0.5 noradrenaline;
1. **G(Aggressiveness Intensity)** = 0.5 serotonin + 1.0 dopamine + 1.0 noradrenaline;

##Cognitive architecture analysis

##Conclusion