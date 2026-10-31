import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import './style/index.css'
import App from './App.jsx'
import { StopsProvider } from './context/StopsContext.jsx'
import { ParametersProvider } from './context/ParametersContext.jsx'

createRoot(document.getElementById('root')).render(
  <StrictMode>
    <StopsProvider>
      <ParametersProvider>
        <App />
      </ParametersProvider>
    </StopsProvider>
  </StrictMode>,
)
