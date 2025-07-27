import React, { useState } from 'react';
import styles from './TerminalCommand.module.css';

interface TerminalCommandProps {
  command: string;
  output?: string;
  user?: string;
  host?: string;
  directory?: string;
}

const TerminalCommand: React.FC<TerminalCommandProps> = ({ 
  command, 
  output, 
  user = "level00", 
  host = "OverRide", 
  directory = "~" 
}) => {
  const [copied, setCopied] = useState(false);

  const copyToClipboard = () => {
    navigator.clipboard.writeText(command);
    setCopied(true);
    setTimeout(() => setCopied(false), 2000);
  };

  return (
    <div className={styles.terminalWindow}>
      <div className={styles.terminalHeader}>
        <div className={styles.terminalButtons}>
          <div className={styles.terminalButton}></div>
          <div className={styles.terminalButton}></div>
          <div className={styles.terminalButton}></div>
        </div>
        <div className={styles.terminalTitle}>{user}@{host}:{directory}</div>
        <button className={styles.copyButton} onClick={copyToClipboard}>
          {copied ? '✓' : '📋'}
        </button>
      </div>
      <div className={styles.terminalBody}>
        <div className={styles.commandLine}>
          <span className={styles.prompt}>{user}@{host}:{directory}$</span>
          <span className={styles.command}>{command}</span>
        </div>
        {output && (
          <div className={styles.output}>
            {output.split('\n').map((line, index) => (
              <div key={index} className={styles.outputLine}>
                {line}
              </div>
            ))}
          </div>
        )}
      </div>
    </div>
  );
};

export default TerminalCommand;
