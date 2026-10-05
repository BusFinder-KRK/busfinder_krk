import { createContext } from "react";
import { useState } from "react";
import { useEffect } from "react";

export const StopsContext = createContext({
    stops: [],
    isLoading: true,
});


export function StopsProvider({ children }) {
    const [stops, setStops] = useState([]);
    const [isLoading, setLoading] = useState(true);



    useEffect( () => { 
        async function getStops() {
            try {
                const response = await fetch("/api/stops");
                if(!response.ok) throw new Error("Failed to fetch.");
                const data = await response.json();
                for(const entry of data["stoplist"]) {
                    console.log(entry);
                }
                setStops(data["stoplist"]);
            } catch (error) {
                console.error("Failed to load stops: ", error);
            } finally {
                setLoading(false);
            }
        }
        getStops(); 
    }, []);
    return (
        <StopsContext value={{ stops, isLoading }}>
            {children}
        </StopsContext>
    )
}

export default StopsContext;