import { createContext, useState } from "react";

export const ParametersContext = createContext({
    parameters: {},
    setParameters: () => { },
})


export function ParametersProvider({ children }) {
    const [parameters, setParameters] = useState({
        searchWindow: 30,
        timeForChange: 0.5,
        walkingMultiplier: 1.01,
        adjacentStops: 40
    });
    return (
        <ParametersContext value={{ parameters, setParameters }}>
            {children}
        </ParametersContext>
    )

}

export default ParametersContext;