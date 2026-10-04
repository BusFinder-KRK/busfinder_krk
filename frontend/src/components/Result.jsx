import ResultNodeGetOn from "./minor/ResultNodeGetOn";
import ResultNodeGetOff from "./minor/ResultNodeGetOff";
import ResultNodeWalk from "./minor/ResultNodeWalk";
import Skeleton, { SkeletonTheme } from "react-loading-skeleton";

import "../style/components/Result.css";
import "react-loading-skeleton/dist/skeleton.css";


export default function Result({ routeResult, invalid, isSubmitting }) {

    if(invalid) {
        return (
            <div className="result">
                <p className="invalid-input">
                    Please choose one of the available stop names
                </p>
            </div>
        );
    }

    if(isSubmitting) {
        return(
            <SkeletonTheme baseColor="#2a2238" highlightColor="#bb7428">
                <div className="result skeleton-wrapper"> 
                    <h3> <Skeleton
                    count={1}
                    inline={false}
                     /> </h3>
                    <Skeleton
                    count={15}
                    inline={false}
                     />
                </div>
            </SkeletonTheme>
        )
    }


    if(routeResult) {
        function calculateDuration(startTime, endTime) {
            const [startHours, startMinutes, startSeconds] = startTime.split(":").map(Number);
            const [endHours, endMinutes, endSeconds] = endTime.split(":").map(Number);
            const startTotalMinutes = startHours * 60 + startMinutes;
            const endTotalMinutes = endHours * 60 + endMinutes;
            return endTotalMinutes - startTotalMinutes;
        }

        const startStop = routeResult.route_proper[0].stop_name;
        const endStop = routeResult.route_proper[routeResult.route_proper.length - 1].stop_name;
        
        const compArray = new Array();
        for(let i = 0; i < routeResult.route_proper.length; i++) {
            if(routeResult.route_proper[i].route_name === "walk" && routeResult.route_proper[i].type === "GET_OFF") {
                if(i === 0) {
                    throw new Error("'walk' node at the beginning of array with the 'GET_OFF' type; most likely invalid data returned from backend.");
                }
                let duration = calculateDuration(routeResult.route_proper[i - 1].time, routeResult.route_proper[i].time);
                compArray.push((
                    <ResultNodeWalk resultEntry={routeResult.route_proper[i]} duration={duration} key={i} />
                ));
            } 
            else if (routeResult.route_proper[i].route_name !== "walk" && 
            routeResult.route_proper[i].type === "GET_ON") {
                compArray.push((
                    <ResultNodeGetOn resultEntry={routeResult.route_proper[i]} key={i} />
                ));

            }
            else if (routeResult.route_proper[i].route_name !== "walk" && 
            routeResult.route_proper[i].type === "GET_OFF") {
                compArray.push((
                    <ResultNodeGetOff resultEntry={routeResult.route_proper[i]} key={i} />
                ));
            }
        }

        return (
        <div className="result">
            <h3>
                Result
            </h3>
            <ul className="result-header">
                <li>
                    Route from {startStop} to {endStop}
                </li>
                <li>
                    Total time: {calculateDuration(routeResult.start_time, routeResult.end_time)} minutes
                </li>
            </ul>
            <ul className="result-list">
                {compArray}
            </ul>
        </div>
        )
    }
    
}