export default function({ resultEntry, duration }) {
    const timeNoun = duration == 1 ? "minute" : "minutes";
    return (
        <li>
            <p>
                Walk for {duration} {timeNoun}.
            </p>
        </li>
    );
}