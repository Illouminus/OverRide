import type {ReactNode} from 'react';
import clsx from 'clsx';
import Link from '@docusaurus/Link';
import useDocusaurusContext from '@docusaurus/useDocusaurusContext';
import Layout from '@theme/Layout';
import HomepageFeatures from '@site/src/components/HomepageFeatures';
import Heading from '@theme/Heading';

import styles from './index.module.css';

function HomepageHeader() {
  const {siteConfig} = useDocusaurusContext();
  return (
    <header className={clsx('hero', styles.heroBanner)}>
      <div className="container">
        <div className={styles.heroContent}>
          <div className={styles.terminalWindow}>
            <div className={styles.terminalHeader}>
              <div className={styles.terminalButtons}>
                <div className={styles.terminalButton}></div>
                <div className={styles.terminalButton}></div>
                <div className={styles.terminalButton}></div>
              </div>
              <div className={styles.terminalTitle}>root@42school:~/OverRide</div>
            </div>
            <div className={styles.terminalBody}>
              <div className={styles.terminalLine}>
                <span className={styles.prompt}>root@42school:~/OverRide#</span>
                <span className={styles.command}> ./start_challenge</span>
              </div>
              <div className={styles.terminalOutput}>
                <div className={styles.glitch} data-text="OverRide">
                  <Heading as="h1" className={styles.heroTitle}>
                    OverRide
                  </Heading>
                </div>
                <p className={styles.heroSubtitle}>
                  🏴‍☠️ Master the art of binary exploitation at 42 School
                </p>
                <div className={styles.skillTags}>
                  <span className={styles.tag}>Buffer Overflow</span>
                  <span className={styles.tag}>Format String</span>
                  <span className={styles.tag}>Return-to-libc</span>
                  <span className={styles.tag}>ROP Chains</span>
                </div>
              </div>
            </div>
          </div>
          <div className={styles.buttons}>
            <Link
              className={clsx("button button--lg", styles.hackButton)}
              to="/docs/intro">
              🚀 Start Hacking
            </Link>
            <Link
              className={clsx("button button--outline button--lg", styles.learnButton)}
              to="/docs/level00">
              📚 View Levels
            </Link>
          </div>
        </div>
      </div>
      <div className={styles.matrixRain}></div>
    </header>
  );
}

export default function Home(): ReactNode {
  const {siteConfig} = useDocusaurusContext();
  return (
    <Layout
      title={`${siteConfig.title} - Binary Exploitation Challenges`}
      description="Master binary exploitation techniques with OverRide challenges from 42 School. Learn buffer overflows, format strings, ROP chains and more.">
      <HomepageHeader />
      <main>
        <HomepageFeatures />
      </main>
    </Layout>
  );
}
