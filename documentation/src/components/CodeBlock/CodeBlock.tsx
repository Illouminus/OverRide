import React, { useState } from 'react';
import styles from './CodeBlock.module.css';

interface CodeBlockProps {
  command?: string;
  output?: string;
  language?: string;
  title?: string;
}

const CodeBlock: React.FC<CodeBlockProps> = ({ command, output, language = 'bash', title }) => {
  const [copied, setCopied] = useState(false);

  const copyToClipboard = () => {
    const textToCopy = command || output || '';
    navigator.clipboard.writeText(textToCopy);
    setCopied(true);
    setTimeout(() => setCopied(false), 2000);
  };

  return (
    <div className={styles.codeContainer}>
      {title && (
        <div className={styles.codeHeader}>
          <span className={styles.codeTitle}>{title}</span>
          <button className={styles.copyButton} onClick={copyToClipboard}>
            {copied ? '✓' : 'Copy'}
          </button>
        </div>
      )}
      <div className={styles.codeContent}>
        {command && (
          <div className={styles.commandSection}>
            <span className={styles.prompt}>$</span>
            <span className={styles.command}>{command}</span>
          </div>
        )}
        {output && (
          <pre className={styles.output}>{output}</pre>
        )}
      </div>
    </div>
  );
};

export default CodeBlock;
