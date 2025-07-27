import React, { useEffect, useRef } from 'react';

interface AsciinemaPlayerProps {
  src: string;
  id: string;
}

const AsciinemaPlayer: React.FC<AsciinemaPlayerProps> = ({ src, id }) => {
  const containerRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    const script = document.createElement('script');
    script.src = src;
    script.id = id;
    script.async = true;
    
    if (containerRef.current) {
      containerRef.current.appendChild(script);
    }

    return () => {
      // Cleanup script when component unmounts
      const existingScript = document.getElementById(id);
      if (existingScript) {
        existingScript.remove();
      }
    };
  }, [src, id]);

  return <div ref={containerRef} />;
};

export default AsciinemaPlayer;
