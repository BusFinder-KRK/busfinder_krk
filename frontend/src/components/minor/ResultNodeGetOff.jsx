export default function ResultNodeGetOff({ resultEntry }) {
    return (
        <li className="result-node-get-off">
            <p>
                <strong>[{resultEntry.time}] </strong> 
                Get off {resultEntry.route_name} at {resultEntry.stop_name}
            </p>
        </li>
    )

}