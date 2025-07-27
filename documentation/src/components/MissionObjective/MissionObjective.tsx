import React from 'react';
import styles from './MissionObjective.module.css';

interface MissionObjectiveProps {
  target: string;
  method: string;
  level: string;
}

const MissionObjective: React.FC<MissionObjectiveProps> = ({ target, method, level }) => {
  return (
    <div className={styles.missionCard}>
      <div className={styles.missionHeader}>
        <span className={styles.levelBadge}>{level}</span>
        <h2 className={styles.missionTitle}>Mission Objective</h2>
      </div>
      <div className={styles.missionContent}>
        <p className={styles.missionText}>
          Gain access to user <code className={styles.targetCode}>{target}</code> by exploiting {method}.
        </p>
      </div>
    </div>
  );
};

export default MissionObjective;
