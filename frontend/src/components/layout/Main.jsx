import Form from "../Form"
import Result from "../Result";
import StopsContext from "../../context/StopsContext";
import Sidebar from "../Sidebar";
import TransitMap from "../TransitMap";

import ParametersContext from "../../context/ParametersContext";

import { useState } from "react"
import { useContext } from "react"
import { Group, Panel, Separator } from "react-resizable-panels"

import "../../style/components/layout/Main.css"


export default function Main() {

    const [startInput, setStartInput] = useState({ id: "", name: "" });
    const [endInput, setEndInput] = useState({ id: "", name: "" });
    const [datetime, setDatetime] = useState(new Date());
    const { parameters, setParameters } = useContext(ParametersContext);

    const [isSubmitting, setIsSubmitting] = useState(false);
    const [routeResult, setRouteResult] = useState(null);
    const [invalid, setInvalid] = useState(false);

    const { stops = [], isLoading } = useContext(StopsContext);

    function formatTime(datetime) {
        const formatter = new Intl.DateTimeFormat('pl-PL', {
            timeZone: 'Europe/Warsaw',
            year: 'numeric',
            month: '2-digit',
            day: '2-digit',
            hour: '2-digit',
            minute: '2-digit',
            second: '2-digit',
            hour12: false,
        });

        const parts = Object.fromEntries(
            formatter.formatToParts(datetime).map((p) => [p.type, p.value])
        );

        return `${parts.year}-${parts.month}-${parts.day} ${parts.hour}:${parts.minute}:${parts.second}`;
    }

    async function handleSubmit(e) {
        e.preventDefault();
        if (isLoading) return;
        setInvalid(false);
        if (!stops.includes(startInput) || !stops.includes(endInput)) {
            setInvalid(true);
            setRouteResult(null);
            return;
        }
        const formattedDate = formatTime(datetime);

        setIsSubmitting(true);

        const params = new URLSearchParams({
            "stopid1": startInput.id,
            "stopid2": endInput.id,
            "datetime": formattedDate,
            "search_window": parameters["searchWindow"],
            "time_for_change": parameters["timeForChange"],
            "walking_multiplier": parameters["walkingMultiplier"],
            "adjacent_stops": parameters["adjacentStops"]

        });
        try {
            const response = await fetch(`/api/route/route_stops?${params.toString()}`);
            if (!response.ok) {
                throw new Error("Failed to submit data.");
            }
            const data = await response.json();
            setRouteResult(data);
            (data);
        } catch (error) {
            console.error("Error fetching route: ", error);
        } finally {
            setIsSubmitting(false);
        }
    }

    return (
        <div className="main">
            <Group>
                <Panel className="route-search" defaultSize="25%" maxSize="25%" minSize="25%">
                    <Form
                        startInput={startInput}
                        endInput={endInput}
                        setStartInput={setStartInput}
                        setEndInput={setEndInput}
                        handleSubmit={handleSubmit}
                        setDatetime={setDatetime}
                        datetime={datetime}
                    />
                    <Result routeResult={routeResult} invalid={invalid} isSubmitting={isSubmitting} />
                </Panel>
                <Panel className="transit-map">
                    <div className="map-inner-wrapper">
                        <TransitMap />
                    </div>
                </Panel>
                <Separator />
                <Panel className="sidebar" defaultSize="25%" maxSize="40%" minSize="15%">
                    <Sidebar />
                </Panel>
            </Group >
        </div >
    );
}