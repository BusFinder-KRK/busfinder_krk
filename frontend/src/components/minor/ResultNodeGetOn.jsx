export default function ResultNodeGetOn({ resultEntry }) {
    return (
        <li className="result-node-get-on">
            <p>
                <strong>[{resultEntry.time}] </strong> 
                Get on {resultEntry.route_name} at {resultEntry.stop_name}
            </p>
        </li>
    )

}