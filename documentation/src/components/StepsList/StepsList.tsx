import React from 'react';
import styles from './StepsList.module.css';

interface Step {
  title: string;
  description: string;
  command?: string;
  output?: string;
}

interface StepsListProps {
  steps: Step[];
}

const StepsList: React.FC<StepsListProps> = ({ steps }) => {
  return (
    <div className={styles.stepsContainer}>
      {steps.map((step, index) => (
        <div key={index} className={styles.step}>
          <div className={styles.stepHeader}>
            <span className={styles.stepNumber}>{index + 1}</span>
            <h3 className={styles.stepTitle}>{step.title}</h3>
          </div>
          
          <p className={styles.stepDescription}>{step.description}</p>
          
          {step.command && (
            <div className={styles.codeBlock}>
              <div className={styles.commandLine}>
                <span className={styles.prompt}>$</span>
                <code className={styles.command}>{step.command}</code>
              </div>
              {step.output && (
                <pre className={styles.output}>{step.output}</pre>
              )}
            </div>
          )}
        </div>
      ))}
    </div>
  );
};

export default StepsList;
