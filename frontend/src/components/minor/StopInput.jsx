import { use, useContext, useEffect, useRef, useState } from "react";
import StopsContext from "../../context/StopsContext";
import "../../style/components/Form.css"

function StopInput({ inputType, input, setInput }) {
    const [query, setQuery] = useState("");
    const [suggestions, setSuggestions] = useState([]);
    const [isOpen, setIsOpen] = useState(false);

    const { stops = [], isLoading } = useContext(StopsContext);
    const containerRef = useRef(null)

    useEffect(() => {
        function handleClickOutside(event) {
            if (containerRef.current && !containerRef.current.contains(event.target)) {
                setIsOpen(false)
            }
        }

        document.addEventListener("mousedown", handleClickOutside);
        return () => {
            document.removeEventListener("mousedown", handleClickOutside);
        };
    }, []);


    function handleInput(e) {
        const value = e.target.value;
        setQuery(value);
        if(input != "" && value != input) {
            setInput({id: "", name: ""});
        }
        if(query.trim().length > 1 && !isLoading) {
            const filtered = stops.filter( 
                (stop) => 
                    stop.name.toLowerCase().includes(value.toLowerCase())
            );
            setSuggestions(filtered)
        } else {
            setSuggestions([]);
        }
    }


    function handleFocus() {
        setIsOpen(true)
    }

    function handleSelect(suggestion) {
        setInput(suggestion);
        setQuery(suggestion.name);
        setSuggestions([]);
    }


    return (
        <div className="stop-input" ref={containerRef}>
            <input 
                type="text" 
                name={inputType}
                id={inputType}
                value={query} 
                autoComplete="off"
                onChange={handleInput}
                onFocus={handleFocus}
                placeholder={`Input ${inputType} stop`}
            />
            {isOpen && suggestions.length > 0 && (
                <ul className="autocomplete-search">
                    {
                        suggestions.map((item) => (
                            <li key={item.id} onClick={() => handleSelect(item) }>
                                {item.name}
                            </li>
                        ))
                    }
                </ul> 
            )}
       </div>
    );
}

export default StopInput