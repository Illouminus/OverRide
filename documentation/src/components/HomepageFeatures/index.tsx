import type {ReactNode} from 'react';
import clsx from 'clsx';
import Heading from '@theme/Heading';
import styles from './styles.module.css';

type FeatureItem = {
  title: string;
  icon: string;
  description: ReactNode;
};

const FeatureList: FeatureItem[] = [
  {
    title: '🛡️ Binary Exploitation',
    icon: '💀',
    description: (
      <>
        Master advanced techniques like buffer overflows, format string bugs,
        and memory corruption exploits. Learn to bypass modern security mechanisms
        and understand how attackers think.
      </>
    ),
  },
  {
    title: '🎯 Progressive Difficulty',
    icon: '📈',
    description: (
      <>
        9 carefully crafted levels that gradually increase in complexity.
        Start with basic overflows and progress to advanced ROP chains,
        ASLR bypasses, and stack canary defeats.
      </>
    ),
  },
  {
    title: '🧠 42 School Methodology',
    icon: '🎓',
    description: (
      <>
        Experience the peer-to-peer learning approach of 42 School.
        No courses, no teachers - just you, the binaries, and your determination
        to understand how systems can be compromised.
      </>
    ),
  },
  {
    title: '🔬 Reverse Engineering',
    icon: '🔍',
    description: (
      <>
        Analyze compiled binaries, understand assembly code, and discover
        vulnerabilities through static and dynamic analysis. Tools like GDB,
        objdump, and strace become your best friends.
      </>
    ),
  },
  {
    title: '⚡ Real-World Skills',
    icon: '🌐',
    description: (
      <>
        Develop practical cybersecurity skills used in penetration testing,
        bug bounty hunting, and security research. Understanding exploitation
        is key to building better defenses.
      </>
    ),
  },
  {
    title: '🏆 Hands-On Learning',
    icon: '⚒️',
    description: (
      <>
        No theory without practice. Every level requires you to gain root access
        by exploiting real vulnerabilities. Code, debug, exploit, and learn
        from your mistakes in a safe environment.
      </>
    ),
  },
];

function Feature({title, icon, description}: FeatureItem) {
  return (
    <div className={clsx('col col--4 margin-bottom--lg', styles.feature)}>
      <div className={styles.featureCard}>
        <div className={styles.featureIcon}>
          <span className={styles.iconEmoji}>{icon}</span>
        </div>
        <div className={styles.featureContent}>
          <Heading as="h3" className={styles.featureTitle}>{title}</Heading>
          <p className={styles.featureDescription}>{description}</p>
        </div>
      </div>
    </div>
  );
}

export default function HomepageFeatures(): ReactNode {
  return (
    <section className={styles.features}>
      <div className="container">
        <div className={styles.sectionHeader}>
          <Heading as="h2" className={styles.sectionTitle}>
            🎮 What You'll Master
          </Heading>
          <p className={styles.sectionSubtitle}>
            OverRide challenges will push your limits and transform you into a skilled exploit developer
          </p>
        </div>
        <div className="row">
          {FeatureList.map((props, idx) => (
            <Feature key={idx} {...props} />
          ))}
        </div>
      </div>
    </section>
  );
}
