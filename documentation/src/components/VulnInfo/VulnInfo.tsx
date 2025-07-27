import React from 'react';
import styles from './VulnInfo.module.css';

interface VulnInfoProps {
  type: string;
  severity: 'low' | 'medium' | 'high' | 'critical';
  description: string;
  impact: string[];
  techniques: string[];
}

const VulnInfo: React.FC<VulnInfoProps> = ({ type, severity, description, impact, techniques }) => {
  const getSeverityClass = (severity: string) => {
    return styles[`severity${severity.charAt(0).toUpperCase() + severity.slice(1)}`];
  };

  const getSeverityIcon = (severity: string) => {
    switch (severity) {
      case 'low': return '🟢';
      case 'medium': return '🟡';
      case 'high': return '🟠';
      case 'critical': return '🔴';
      default: return '⚪';
    }
  };

  return (
    <div className={styles.vulnCard}>
      <div className={styles.vulnHeader}>
        <div className={styles.vulnType}>
          <span className={styles.vulnIcon}>🛡️</span>
          <h3 className={styles.vulnTitle}>{type}</h3>
        </div>
        <div className={`${styles.severityBadge} ${getSeverityClass(severity)}`}>
          <span className={styles.severityIcon}>{getSeverityIcon(severity)}</span>
          <span className={styles.severityText}>{severity.toUpperCase()}</span>
        </div>
      </div>
      
      <div className={styles.vulnContent}>
        <div className={styles.description}>
          <h4 className={styles.sectionTitle}>📝 Description</h4>
          <p className={styles.descriptionText}>{description}</p>
        </div>
        
        <div className={styles.impact}>
          <h4 className={styles.sectionTitle}>💥 Impact</h4>
          <ul className={styles.impactList}>
            {impact.map((item, index) => (
              <li key={index} className={styles.impactItem}>
                <span className={styles.bullet}>▶</span>
                {item}
              </li>
            ))}
          </ul>
        </div>
        
        <div className={styles.techniques}>
          <h4 className={styles.sectionTitle}>🔧 Techniques Used</h4>
          <div className={styles.techniquesTags}>
            {techniques.map((technique, index) => (
              <span key={index} className={styles.techniqueTag}>
                {technique}
              </span>
            ))}
          </div>
        </div>
      </div>
    </div>
  );
};

export default VulnInfo;
