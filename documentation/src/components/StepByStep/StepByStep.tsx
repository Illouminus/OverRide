import React, { useState } from 'react';
import styles from './StepByStep.module.css';

interface Step {
  title: string;
  description: string;
  command?: string;
  output?: string;
  tip?: string;
  warning?: string;
}

interface StepByStepProps {
  steps: Step[];
}

const StepByStep: React.FC<StepByStepProps> = ({ steps }) => {
  const [completedSteps, setCompletedSteps] = useState<number[]>([]);
  const [expandedStep, setExpandedStep] = useState<number | null>(0);

  const toggleStep = (stepIndex: number) => {
    if (completedSteps.includes(stepIndex)) {
      setCompletedSteps(completedSteps.filter(i => i !== stepIndex));
    } else {
      setCompletedSteps([...completedSteps, stepIndex]);
    }
  };

  const expandStep = (stepIndex: number) => {
    setExpandedStep(expandedStep === stepIndex ? null : stepIndex);
  };

  return (
    <div className={styles.stepContainer}>
      <div className={styles.progressBar}>
        <div 
          className={styles.progressFill}
          style={{ width: `${(completedSteps.length / steps.length) * 100}%` }}
        />
      </div>
      
      {steps.map((step, index) => (
        <div 
          key={index}
          className={`${styles.step} ${completedSteps.includes(index) ? styles.completed : ''}`}
        >
          <div 
            className={styles.stepHeader}
            onClick={() => expandStep(index)}
          >
            <div className={styles.stepNumber}>
              {completedSteps.includes(index) ? '✓' : index + 1}
            </div>
            <h3 className={styles.stepTitle}>{step.title}</h3>
            <div 
              className={`${styles.expandIcon} ${expandedStep === index ? styles.expanded : ''}`}
            >
              ▼
            </div>
          </div>
          
          {expandedStep === index && (
            <div className={styles.stepContent}>
              <p className={styles.stepDescription}>{step.description}</p>
              
              {step.command && (
                <div className={styles.commandBlock}>
                  <div className={styles.commandHeader}>Command:</div>
                  <code className={styles.command}>{step.command}</code>
                </div>
              )}
              
              {step.output && (
                <div className={styles.outputBlock}>
                  <div className={styles.outputHeader}>Expected Output:</div>
                  <pre className={styles.output}>{step.output}</pre>
                </div>
              )}
              
              {step.tip && (
                <div className={styles.tip}>
                  <span className={styles.tipIcon}>💡</span>
                  <span className={styles.tipText}>{step.tip}</span>
                </div>
              )}
              
              {step.warning && (
                <div className={styles.warning}>
                  <span className={styles.warningIcon}>⚠️</span>
                  <span className={styles.warningText}>{step.warning}</span>
                </div>
              )}
              
              <button 
                className={styles.completeButton}
                onClick={() => toggleStep(index)}
              >
                {completedSteps.includes(index) ? 'Mark as Incomplete' : 'Mark as Complete'}
              </button>
            </div>
          )}
        </div>
      ))}
    </div>
  );
};

export default StepByStep;
